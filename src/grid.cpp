#include "grid.h"
#include "u_types.h"

int dual_grid_t::init(linear_arena_t* arena, glm::ivec2 global_min, glm::ivec2 global_max, u32 loose_cell_size,
		      u32 tight_cell_size)
{
	grid_min_x = global_min.x;
	grid_min_y = global_min.y;
	inv_tile_size_loose = 1.0f / (float)loose_cell_size;
	inv_tile_size_tight = 1.0f / (float)tight_cell_size;

	u64 handle;
	u32 columns = ceil(global_max.x - global_min.x);
	u32 rows = ceil(global_max.y - global_min.y);

	// Loose grid

	loose_grid.num_columns = ceil(columns * inv_tile_size_loose);
	loose_grid.num_rows = ceil(rows * inv_tile_size_loose);
	int res = arena_alloc(arena, &handle, sizeof(loose_cell_t) * loose_grid.num_columns * loose_grid.num_rows);
	if(res == EXIT_FAILURE)
	{
		printf("Grid: could not allocate loose cells\n");
		return EXIT_FAILURE;
	}

	loose_grid.cells = (loose_cell_t*)pointer_from_arena(arena, handle);
	for(u32 i = 0; i < loose_grid.num_columns * loose_grid.num_rows; i++)
	{
		loose_grid.cells[i] = {-1, 0.0f, 0.0f, {0.0f,0.0f}};
	}

	// Tight grid

	tight_grid.num_columns = ceil(columns * inv_tile_size_tight);
	tight_grid.num_rows = ceil(rows * inv_tile_size_tight);
	res = arena_alloc(arena, &handle, sizeof(tight_cell_t) * tight_grid.num_columns * tight_grid.num_rows);
	if(res == EXIT_FAILURE)
	{
		printf("Grid: could not allocate tight cells\n");
		return EXIT_FAILURE;
	}

	tight_grid.cells = (tight_cell_t*)pointer_from_arena(arena, handle);
	for(u32 i = 0; i < tight_grid.num_columns * tight_grid.num_rows; i++)
	{
		tight_grid.cells[i] = {-1};
	}

	// Elements

	res = arena_alloc(arena, &handle, sizeof(grid_element_t) * PROGRAM_MAX_OBJECTS);
	if(res == EXIT_FAILURE)
	{
		printf("Grid: could not allocate element array\n");
		return EXIT_FAILURE;
	}

	elements = (grid_element_t*)pointer_from_arena(arena, handle);
	for(u32 i = 0; i < PROGRAM_MAX_OBJECTS; i++)
	{
		elements[i] = {0, -1};
	}

	// Nodes

	res = arena_alloc(arena, &handle, sizeof(tight_node_t) * PROGRAM_MAX_OBJECTS);
	if(res == EXIT_FAILURE)
	{
		printf("Grid: could not allocate tight node array\n");
		return EXIT_FAILURE;
	}

	nodes = (tight_node_t*)pointer_from_arena(arena, handle);
	for(u32 i = 0; i < PROGRAM_MAX_OBJECTS; i++)
	{
		nodes[i] = {0, -1};
	}

	return EXIT_SUCCESS;
}

void dual_grid_expand_aabb(dual_grid_t* grid, loose_cell_t* loose_cell, u32 loose_cell_index, glm::vec2 loose_aabb_min,
			   glm::vec2 loose_aabb_max, glm::vec2 grid_aabb_min, glm::vec2 grid_aabb_max)
{
	float new_half_height = abs((grid_aabb_max.y - grid_aabb_min.y) * 0.5);
	float new_half_width = abs((grid_aabb_max.x - grid_aabb_min.x) * 0.5);

	if((new_half_height <= loose_cell->half_height) && (new_half_width <= loose_cell->half_width))
	{
		return;
	}

	u32 old_min_x_tile = floor((loose_aabb_min.x - grid->grid_min_x) * grid->inv_tile_size_tight);
	u32 old_min_y_tile = floor((loose_aabb_min.y - grid->grid_min_y) * grid->inv_tile_size_tight);
	u32 old_max_x_tile = floor((loose_aabb_max.x - grid->grid_min_x) * grid->inv_tile_size_tight);
	u32 old_max_y_tile = floor((loose_aabb_max.y - grid->grid_min_y) * grid->inv_tile_size_tight);
	u32 new_min_x_tile = floor((grid_aabb_min.x - grid->grid_min_x) * grid->inv_tile_size_tight);
	u32 new_min_y_tile = floor((grid_aabb_min.y - grid->grid_min_y) * grid->inv_tile_size_tight);
	u32 new_max_x_tile = floor((grid_aabb_max.x - grid->grid_min_x) * grid->inv_tile_size_tight);
	u32 new_max_y_tile = floor((grid_aabb_max.y - grid->grid_min_y) * grid->inv_tile_size_tight);

	for(u32 y = new_min_y_tile; y <= new_max_y_tile; y++)
	{
		bool y_in_old_bounds = ((old_min_y_tile < y) && (y < old_max_y_tile));

		for(u32 x = new_min_x_tile; x <= new_max_x_tile; x++)
		{
			if(y_in_old_bounds && ((old_min_x_tile < x) && (x < old_max_x_tile)))
			{
				continue; // Don't add duplicate loose entries into tight cells
			}

			u32 node_index = grid->node_freelist.pop();

			tight_cell_t* tight_cell = &grid->tight_grid.cells[(y * grid->tight_grid.num_columns) + x];
			if(tight_cell->first_node < 0)
			{
				tight_cell->first_node = node_index;
			}
			else
			{
				u32 temp = tight_cell->first_node;
				grid->nodes[node_index].next_node = temp;
				tight_cell->first_node = node_index;
			}

			grid->nodes[node_index].loose_cell_index = loose_cell_index;
		}
	}

	loose_cell->half_height = new_half_height;
	loose_cell->half_width = new_half_width;
	return;
}

/** Operates on worldspace bounding boxes */
int dual_grid_insert(dual_grid_t* grid, glm::vec4 world_aabb_min, glm::vec4 world_aabb_max, u32 node_id)
{
	glm::vec4 world_center = (world_aabb_min + world_aabb_max) * 0.5f;

	u32 tile_x = floor((world_center.x - grid->grid_min_x) * grid->inv_tile_size_loose);
	u32 tile_y = floor((world_center.y - grid->grid_min_y) * grid->inv_tile_size_loose);

	if((tile_x > grid->loose_grid.num_columns) || (tile_y > grid->loose_grid.num_rows))
	{
		printf("Grid: element world-space position {%f, %f, %f, %f} out of grid bounds", world_center.x,
		       world_center.y, world_center.z, world_center.w);
		return EXIT_FAILURE;
	}

	u32 element_index = grid->element_freelist.pop();

	u32 loose_cell_index = (tile_y * grid->loose_grid.num_columns) + tile_x;
	loose_cell_t* loose_cell = &grid->loose_grid.cells[loose_cell_index];
	if(loose_cell->first_element < 0)
	{
		loose_cell->first_element = element_index;
	}
	else
	{
		u32 temp = loose_cell->first_element;
		loose_cell->first_element = element_index;
		grid->elements[element_index].next_element = temp;
	}

	grid->elements[element_index].node_id = node_id;

	glm::vec2 loose_aabb_min = {loose_cell->center.x - loose_cell->half_width,
				    loose_cell->center.y - loose_cell->half_height};
	glm::vec2 loose_aabb_max = {loose_cell->center.x + loose_cell->half_width,
				    loose_cell->center.y + loose_cell->half_height};
	glm::vec2 grid_aabb_min = glm::min(loose_aabb_min, {world_aabb_min.x, world_aabb_min.y});
	glm::vec2 grid_aabb_max = glm::min(loose_aabb_max, {world_aabb_max.x, world_aabb_max.y});

	dual_grid_expand_aabb(grid, loose_cell, loose_cell_index, loose_aabb_min, loose_aabb_max, grid_aabb_min,
			      grid_aabb_max);

	return EXIT_SUCCESS;
}

void dual_grid_remove(dual_grid_t* grid, u32 node_id, glm::vec2 center);
void dual_grid_move(dual_grid_t* grid, u32 node_id, glm::vec2 new_center);
void dual_grid_optimize(int usage_flag);

void dual_grid_frustum_cull(const dual_grid_t& grid, command_buffer_t* command_buffer, const glm::mat4& inverse_vp)
{
	glm::vec4 ndc_coords[8] = {{-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, {-1, 1, -1, 1},
				   {-1, -1, 1, 1},  {1, -1, 1, 1},  {1, 1, 1, 1},  {-1, 1, 1, 1}};

	glm::vec2 xy;
	glm::vec2 xy_min = {FLT_MAX, FLT_MAX};
	glm::vec2 xy_max = {FLT_MIN, FLT_MIN};
	for(int i = 0; i < 8; i++)
	{
		glm::vec4 world_pos = inverse_vp * ndc_coords[i];
		world_pos /= world_pos.w;
		xy.x = world_pos.x;
		xy.y = world_pos.y;
		xy_min = glm::min(xy_min, xy);
		xy_max = glm::max(xy_max, xy);
	}

	u32 min_x_tile = floor((xy_min.x - grid.grid_min_x) * grid.inv_tile_size_tight);
	u32 max_x_tile = floor((xy_max.x - grid.grid_min_x) * grid.inv_tile_size_tight);
	u32 min_y_tile = floor((xy_min.y - grid.grid_min_y) * grid.inv_tile_size_tight);
	u32 max_y_tile = floor((xy_max.y - grid.grid_min_y) * grid.inv_tile_size_tight);

	draw_command_info_t draw_info;
	for(u32 y = min_y_tile; y <= max_y_tile; y++)
	{
		for(u32 x = min_x_tile; x <= max_x_tile; x++)
		{
			i32 node_index = grid.tight_grid.cells[(y * grid.tight_grid.num_columns) + x].first_node;
			while(node_index >= 0)
			{
				tight_node_t tight_cell = grid.nodes[node_index];

				i32 element_index = grid.loose_grid.cells[tight_cell.loose_cell_index].first_element;
				while(element_index >= 0)
				{
					grid_element_t element = grid.elements[element_index];

					draw_info.node_id = element.node_id;
					command_buffer_add_command(command_buffer, draw_info);
					element_index = element.next_element;
				}

				node_index = tight_cell.next_node;
			}
		}
	}
}

/*
	// Calculate frustum planes
	float e2 = view_frustum.focal_length * view_frustum.focal_length;
	float a2 = view_frustum.aspect_ratio * view_frustum.aspect_ratio;
	float x_lr = view_frustum.focal_length / sqrtf(e2 + 1.0f);
	float z_lr = -(1.0f / sqrtf(e2 + 1.0f));
	float y_tb = view_frustum.focal_length / sqrtf(e2 + a2);
	float z_tb = -(view_frustum.aspect_ratio / sqrtf(e2 + a2));

	glm::vec4 frustum_planes[6];
	frustum_planes[0] = {0, 0, -1, -view_frustum.near_plane_distance}; // Near
	frustum_planes[1] = {0, 0, 1, view_frustum.far_plane_distance};	   // Far
	frustum_planes[2] = {x_lr, 0, z_lr, 0};				   // Left
	frustum_planes[3] = {-x_lr, 0, z_lr, 0};			   // Right
	frustum_planes[4] = {0, y_tb, z_tb, 0};				   // Top
	frustum_planes[5] = {0, -y_tb, z_tb, 0};			   // Bottom

	// Transform grid tile center to view space

	// Compare tile center to frustum planes
	// Cull/Write cell contents
*/

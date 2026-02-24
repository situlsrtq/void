#include "grid.h"

void dual_grid_frustum_cull(const dual_grid_t& grid, const camera_info_t& view_frustum, glm::mat4* inverse_vp)
{
	// Get range of grid tiles
	glm::vec4 ndc_coords[8] = {{-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, {-1, 1, -1, 1},
				   {-1, -1, 1, 1},  {1, -1, 1, 1},  {1, 1, 1, 1},  {-1, 1, 1, 1}};

	glm::mat4 inv_vp;
	if(inverse_vp == 0x0)
	{
		glm::mat4 v = glm::lookAt(view_frustum.Position, view_frustum.Position + view_frustum.Eye,
					  view_frustum.UpAxis);
		glm::mat4 p = glm::perspective(view_frustum.h_fov, view_frustum.aspect_ratio,
					       view_frustum.near_plane_distance, view_frustum.far_plane_distance);
		inv_vp = glm::inverse(p * v);
	}
	else
	{
		inv_vp = *inverse_vp;
	}

	glm::vec2 xy;
	glm::vec2 xy_min = {FLT_MAX, FLT_MAX};
	glm::vec2 xy_max = {FLT_MIN, FLT_MIN};
	for(int i = 0; i < 8; i++)
	{
		glm::vec4 world_pos = inv_vp * ndc_coords[i];
		world_pos /= world_pos.w;
		xy.x = world_pos.x;
		xy.y = world_pos.y;
		xy_min = glm::min(xy_min, xy);
		xy_max = glm::max(xy_max, xy);
	}

	float tile_size = grid.tcell_effective_radius * 2.0f;
	int min_x_tile = floor((xy_min.x - grid.grid_min_x) / tile_size);
	int max_x_tile = floor((xy_max.x - grid.grid_min_x) / tile_size);
	int min_y_tile = floor((xy_min.y - grid.grid_min_y) / tile_size);
	int max_y_tile = floor((xy_max.y - grid.grid_min_y) / tile_size);

	for(int i = min_y_tile; i <= max_y_tile; i++)
	{
		for(int t = min_x_tile; t <= max_x_tile; t++)
		{
			i32 node_index = grid.tight_grid.cells[(i * grid.tight_grid.num_columns) + t].first_node;
      			while(node_index >= 0)
      			{
				tight_node_t tight_cell = grid.nodes[node_index];

				i32 element_index = grid.loose_grid.cells[tight_cell.loose_cell_index].first_element;
				while(element_index >= 0)
				{
					grid_element_t element = grid.elements[element_index];

					add_to_draw_list(element.node_id); 
					element_index = element.next_element;
				}

				node_index = tight_cell.next_node;
      			}
		}
	}

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
}

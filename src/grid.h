#ifndef VOID_GRID_H
#define VOID_GRID_H

#include <float.h>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "u_types.h"
#include "u_mem.h"
#include "rendertypes.h"

#define LOOSE_GRID_DEFAULT_SIZE 8
#define TIGHT_GRID_DEFAULT_SIZE 32
#define GRID_STATIC 0
#define GRID_DYNAMIC 1
#define TRIANGLE_DENSITY_BEFORE_DUAL_CULL 1000000

/*
Loose/Tight dual grid scheme means we can treat every object as an infinitely small point located at the object's
center, which is inserted into the loose grid (linked list for nodes within a cell). The loose cell expands/shrinks to
fit its largest member.
 --
 Each cell in the tight grid stores a linked list of the loose cells that currently intersect it.
 --
The camera frustum is checked against the tight grid. In a CPU-only culling scheme, the nodes in the loose cells may
be checked against the frustum as well. In a GPU-driven scheme, all the nodes in the loose grid cells get added to the
command buffer for the GPU to more precisely cull, if desired

Probably a strong argument for queueing a more narrow cull only if the triangle density after broad phase exceeds some
heuristic
*/

struct grid_element_t
{
	u32 node_id;
	i32 next_element;
};

struct loose_cell_t
{
	i32 first_element;
	float half_height, half_width;
	glm::vec2 center;
};

struct tight_node_t
{
	u32 loose_cell_index;
	i32 next_node;
};

struct tight_cell_t
{
	i32 first_node;
};

template<typename T>
struct grid_info_t
{
	T* cells;
	u32 num_columns, num_rows;
};

struct dual_grid_t
{
	grid_info_t<loose_cell_t> loose_grid;
	grid_info_t<tight_cell_t> tight_grid;
	index_free_list_t element_freelist;
	index_free_list_t node_freelist;
	grid_element_t* elements;
	tight_node_t* nodes;
	float inv_tile_size_tight;
	float inv_tile_size_loose;
	float grid_min_x;
	float grid_min_y;

	int init(linear_arena_t* arena, glm::ivec2 global_min, glm::ivec2 global_max, u32 loose_cell_size,
		 u32 tight_cell_size);
	void release();
};

int dual_grid_insert(dual_grid_t* grid, glm::vec4 world_aabb_min, glm::vec4 world_aabb_max, u32 node_id);
void dual_grid_remove(dual_grid_t* grid, u32 node_id, glm::vec2 center);
void dual_grid_move(dual_grid_t* grid, u32 node_id, glm::vec2 new_center);
void dual_grid_optimize(int usage_flag);
void dual_grid_frustum_cull(const dual_grid_t& grid, command_buffer_t* command_buffer, const glm::mat4& inverse_vp);

#endif

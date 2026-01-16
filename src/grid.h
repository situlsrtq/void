#ifndef VOID_GRID_H
#define VOID_GRID_H

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "u_types.h"

#define GRID_STATIC 0
#define GRID_DYNAMIC 1

typedef void* (*alloc_func)(u64);
typedef void (*free_func)(void*);

/*
Loose/Tight dual grid scheme means we can treat every object as an infinitely small point located at the object's
center, which is inserted into the loose grid (linked list for nodes within a cell). The loose cell expands/shrinks to
fit its largest member.
 --
 Each cell in the tight grid stores a linked list of the loose cells that currently intersect it.
 --
The camera frustum is checked against the tight grid. In a CPU-only culling scheme, the nodes in the loose cells can
be checked against the frustum as well. In a GPU-driven scheme, all the nodes in the loose grid cells get added to the
command buffer for the GPU to more precisely cull

Probably a strong argument for queueing a more narrow cull only if the element density after broad phase exceeds some
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
};

struct tight_node_t
{
	i32 loose_cell_index;
	i32 next_node;
};

struct tight_cell_t
{
	i32 first_node;
	u32 center;
};

template<typename T>
struct grid_info_t
{
	T* cells;
	u32 num_columns, num_rows;
	float inverse_cell_size; // All grids are composed of perfectly square cells TODO: default to 8x8 (loose) and 32x32 (tight) for now
};

struct dual_grid_t
{
	grid_info_t<loose_cell_t> loose_grid;
	grid_info_t<tight_cell_t> tight_grid;
	grid_element_t* elements;
	tight_node_t* nodes;
	float tcell_effective_radius;
	i32 grid_min_x; // These values are retained to offset the grids to Quadrant 1
	i32 grid_min_y;

	int init(glm::ivec2 global_min, glm::ivec2 global_max, u32 loose_cell_size, u32 tight_cell_size,
		 alloc_func alloc, free_func free);
	void release();
};

void dual_grid_insert(dual_grid_t* grid, glm::vec2 aabb_min, glm::vec2 aabb_max);
void dual_grid_move(dual_grid_t* grid, u32 node_id, glm::vec2 new_center);
void dual_grid_remove(dual_grid_t* grid, u32 node_id, glm::vec2 center);
void dual_grid_optimize(int usage_flag);
some_kind_of_command_queue dual_grid_frustum_cull(const dual_grid_t& grid, const glm::mat4& view_matrix, const frustum_info_t& frustum);
// find frustum center - CameraPosition + (CameraForward * (near_distance + far_distance) / 2), start in that cell. transform cell center to camera space, if center dot plane <= -radius, cull 

// TODO: Remember to add non-culled nodes to a list, rather than attempting to draw directly using the node_id, which
// will cause cache misses galore

#endif

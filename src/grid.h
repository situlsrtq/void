#ifndef VOID_GRID_H
#define VOID_GRID_H

#include <glm/vec2.hpp>

#include "u_types.h"

#define GRID_STATIC 0
#define GRID_DYNAMIC 1

/*
Loose/Tight dual grid scheme means we can treat every object as an infinitely small point located at the object's
center, which is inserted into the loose grid (linked list for nodes within a cell). The loose cell expands/shrinks to fit
its largest member.
 --
 Each cell in the tight grid stores a linked list of the loose cells that currently intersect it.
 --
The camera frustum is checked against the tight grid. In a CPU-only culling scheme, the nodes in the loose cells can
be checked against the frustum as well. In a GPU-driven scheme, all the nodes in the loose grid cells get added to the command
buffer for the GPU to more precisely cull

Probably a strong argument for queueing a more narrow cull only if the element density after broad phase exceeds some heuristic
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

struct tight_cell_t
{
	// TODO: figure out loose cell tracking
	i32 first_loose_cell;
};

template<typename T>
struct grid_info_t
{
	T* cells;
	i32 num_columns, num_rows;
	float inverse_cell_width, inverse_cell_height;
};

struct dual_grid_t
{
	grid_info_t<loose_cell_t> loose_grid;
	grid_info_t<tight_cell_t> tight_grid;
	grid_element_t* elements;
	i32 grid_min_x, grid_max_x;
	i32 grid_min_y, grid_max_y;
};

// TODO: Remember to add non-culled nodes to a list, rather than attempting to draw directly using the node_id, which
// will cause cache misses galore

#endif

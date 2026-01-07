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
*/

struct loose_grid_element_t
{
	u32 node_id;
	i32 next_element;
	float radius;
};

struct loose_grid_cell_t
{
	i32 first_element;
	float radius;
	glm::vec2 center;
};

struct loose_grid_t
{
	loose_grid_cell_t* grid_cells;
	i32 num_cells;
};

// TODO: Remember to add non-culled nodes to a list, rather than attempting to draw directly using the node_id, which
// will cause cache misses galore

#endif

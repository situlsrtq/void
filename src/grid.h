#ifndef VOID_GRID_H
#define VOID_GRID_H

#include "u_types.h"

/*
 AABB/Fixed Size dual grid scheme means we can treat every object as an infinitely small point located at the object's
center, which is inserted into the AABB grid (linked list for nodes within a cell). The AABB cell expands/shrinks to fit
its largest member.
 --
 Each cell in the Fixed Size grid stores a linked list of the AABB cells that currently intersect it.
 --
The camera frustum is checked against the Fixed Size grid. In a CPU-only culling scheme, the nodes in the AABB cell can
be checked against the frustum as well. In a GPU-driven scheme, all the nodes in the fixed grid get added to the command
buffer for the GPU to more precisely cull
*/

inline u32 encode_morton_32(u16 x, u16 y)
{
	u64 res = x | ((u64)y << 32u);
	res = (res | (res << 8u)) & 0x00ff00ff00ff00ff;
	res = (res | (res << 4u)) & 0x0f0f0f0f0f0f0f0f;
	res = (res | (res << 2u)) & 0x5555555555555555;
	res = (res | (res << 1u)) & 0x3333333333333333;
	return (u32)(res | (res >> 31u));
}

inline void decode_morton_32(u16* x, u16* y, u32 code)
{
	u64 res = (code | (u64)(code << 31u)) & 0x5555555555555555;
	res = (res | (res >> 1u)) & 0x3333333333333333;
	res = (res | (res >> 2u)) & 0x0f0f0f0f0f0f0f0f;
	res = (res | (res >> 4u)) & 0x00ff00ff00ff00ff;
	res = (res | (res >> 8u));
	*x = (u16)res;
	*y = (u16)(res >> 32u);
}

struct loose_grid_element_t
{
	u32 node_id;
	float radius;
};

struct loose_grid_cell_t
{
	u32 first_element;
	u32 count;
	u32 morton_code;
	float radius;
};

struct loose_grid_t
{
	loose_grid_cell_t* grid_cells;
	u32 num_cells;
};

/* TODO: Broad phase culling will use integer x and y coordinates (morton code) - remember to ensure that the Narrow
 * phase uses the node's actual x and y coordinates (which are floats) to position the bounding spheres */

// TODO: Remember to add non-culled nodes to a list, rather than attempting to draw directly using the node_id, which
// will cause cache misses galore

#endif

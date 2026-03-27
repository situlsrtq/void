#ifndef VOID_SCENE_H
#define VOID_SCENE_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "rendertypes.h"
#include "u_types.h"
#include "u_mem.h"

#define VIS_STATUS_VISIBLE 1
#define VIS_STATUS_INVISIBLE 0
#define VIS_STATUS_FREED 2

struct linked_node_t
{
	linked_node_t* next;
	u32 index;
};

struct mesh_info_t
{
	linked_node_t* node_list;
	u32 base_index;
	u32 size;

	void add_node(u32 index);
	void remove_node(u32 index);
};

struct primitive_create_info_t
{
	index_info_t index_info;
	vertex_info_t vertex_info;
	texture_info_t texture_info;
	glm::vec3 color;
	glm::vec3 min_aabb;
	glm::vec3 max_aabb;
	glm::mat3 model_inv_trans;
};

struct node_create_info_t
{
	u32 node_id;
	u32 mesh_index;
};

struct scene_info_t
{
	u32 mesh_position;
	u32 prim_position;
	u32 node_position;

	mesh_info_t mesh[PROGRAM_MAX_OBJECTS];
	primitive_create_info_t prim[PROGRAM_MAX_OBJECTS];
	node_create_info_t node[PROGRAM_MAX_OBJECTS];
	glm::mat4 model_matrix[PROGRAM_MAX_OBJECTS];

	u32 add_mesh(u32 num_primitives);
	void free_mesh(u32 freed_index);
	void add_primitive(const primitive_create_info_t& create_info, u32 index);
	u32 add_node(const node_create_info_t& create_info, const glm::mat4& model_in);
	void free_node(u32 freed_index);

	private:

	// Prevent Push() or Pop() being called outside of provided Add(), Free() functions
	index_free_list_t mesh_list;
	index_free_list_t node_list;
	block_free_list_t prim_list;
};

#endif

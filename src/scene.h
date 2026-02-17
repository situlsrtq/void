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

	void AddNode(u32 index);
	void RemoveNode(u32 index);
};

struct primitive_create_info_t
{
	index_info_t IndexInfo;
	vertex_info_t VertexInfo;
	texture_info_t TexInfo;
	glm::vec3 Color;
	glm::vec3 MinBB;
	glm::vec3 MaxBB;
	glm::mat3 ModelInvTrans;
};

struct node_create_info_t
{
	u32 node_id;
	u32 MeshIndex;
};

struct scene_info_t
{
	u32 MeshPosition;
	u32 PrimPosition;
	u32 NodePosition;

	mesh_info_t Mesh[PROGRAM_MAX_OBJECTS];
	primitive_create_info_t Prim[PROGRAM_MAX_OBJECTS];
	node_create_info_t Node[PROGRAM_MAX_OBJECTS];
	glm::mat4 ModelMatrix[PROGRAM_MAX_OBJECTS];

	u32 AddMesh(u32 num_primitives);
	void FreeMesh(u32 FreedIndex);
	void AddPrimitive(const primitive_create_info_t& CreateInfo, u32 index);
	u32 AddNode(const node_create_info_t& CreateInfo, const glm::mat4& ModelIn);
	void FreeNode(u32 FreedIndex);

	private:

	// Prevent Push() or Pop() being called outside of provided Add(), Free() functions
	index_free_list_t MeshList;
	index_free_list_t NodeList;
	block_free_list_t PrimList;
};

#endif

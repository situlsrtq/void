#ifndef VOID_SCENE_H
#define VOID_SCENE_H

#include <stdint.h>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "rendertypes.h"
#include "u_mem.h"

#define PROGRAM_MAX_OBJECTS 255
#define OBJECT_ALLOC_ERROR PROGRAM_MAX_OBJECTS + 1
#define VIS_STATUS_VISIBLE 1
#define VIS_STATUS_INVISIBLE 0
#define VIS_STATUS_FREED 2

struct linked_node_t
{
	linked_node_t* next;
	uint32_t index;
};

struct mesh_info_t
{
	linked_node_t* node_list;
	uint32_t base_index;
	uint32_t size;

	void AddNode(uint32_t index);
	void RemoveNode(uint32_t index);
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
	uint32_t Visible;
	uint32_t MeshIndex;
};

struct scene_info_t
{
	uint32_t MeshPosition;
	uint32_t PrimPosition;
	uint32_t NodePosition;

	mesh_info_t Mesh[PROGRAM_MAX_OBJECTS];
	primitive_create_info_t Prim[PROGRAM_MAX_OBJECTS];
	node_create_info_t Node[PROGRAM_MAX_OBJECTS];
	glm::mat4 ModelMatrix[PROGRAM_MAX_OBJECTS];

	uint32_t AddMesh(uint32_t num_primitives);
	void FreeMesh(uint32_t FreedIndex);
	void AddPrimitive(const primitive_create_info_t& CreateInfo, uint32_t index);
	uint32_t AddNode(const node_create_info_t& CreateInfo, const glm::mat4& ModelIn);
	void FreeNode(uint32_t FreedIndex);

	private:

	// Prevent Push() or Pop() being called outside of provided Add(), Free() functions
	index_free_list_t MeshList;
	index_free_list_t NodeList;
	block_free_list_t PrimList;
};

#endif

#ifndef VOID_UMEMORY_H
#define VOID_UMEMORY_H

#include <stdint.h>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "rendertypes.h"

#define PROGRAM_MAX_OBJECTS 255
#define OBJECT_ALLOC_ERROR PROGRAM_MAX_OBJECTS+1
#define VIS_STATUS_VISIBLE 1
#define VIS_STATUS_INVISIBLE 0
#define VIS_STATUS_FREED 2

// For internal use, never user-accessible
struct free_index_list_t
{
	uint32_t NextFreePosition = 0;
	uint32_t OpenPositions[PROGRAM_MAX_OBJECTS];

	void Push(uint32_t Index);
	uint32_t Pop();
};

struct free_block_list_t
{
	/*
		Linked List
	*/

	void Push(uint32_t Index);
	uint32_t Pop();
};

// User accessible
struct mesh_create_info_t
{
	/*
		Eventually, a 'mesh' will track/manage the primitives that make it up, and keep a 
		list of the nodes that reference it. But I don't want to deal with that right now 
		and will implement it later
	*/
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
	uint32_t MeshBaseIndex;
	uint32_t NumPrimitives;
};

// User accessible
struct scene_info_t
{
	uint32_t PrimPosition;
	uint32_t NodePosition;

	primitive_create_info_t Prim[PROGRAM_MAX_OBJECTS];
	node_create_info_t Node[PROGRAM_MAX_OBJECTS];
	glm::mat4 ModelMatrix[PROGRAM_MAX_OBJECTS];

	uint32_t AddMesh(const mesh_create_info_t& CreateInfo);
	uint32_t AddPrimitive(const primitive_create_info_t& CreateInfo);
	uint32_t AddNode(const node_create_info_t& CreateInfo, const glm::mat4& ModelIn);
	void FreeNode(uint32_t FreedIndex);

	private:

	// Prevent Push() or Pop() being called outside of provided Alloc(), Free() functions
	free_index_list_t NodeList;
	free_index_list_t PrimList;
};

#endif

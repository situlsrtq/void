#ifndef VOID_UMEMORY_H
#define VOID_UMEMORY_H

#include <stdint.h>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "rendertypes.h"

#define PROGRAM_MAX_OBJECTS 255
#define VIS_STATUS_VISIBLE 1
#define VIS_STATUS_INVISIBLE 0
#define VIS_STATUS_FREED 2

// For internal use, never user-accessible
struct free_list_t
{
	uint8_t NextFreePosition = 0;
	uint8_t OpenPositions[PROGRAM_MAX_OBJECTS];

	void Push(uint8_t Index);
	uint8_t Pop();
};

// User accessible
struct geometry_interleaved_info_t
{
	bool New;
	bool Deleted;
	index_info_t IndexInfo;
	vertex_info_t VertexInfo;
	texture_info_t TexInfo;
	glm::vec3 Color;
	glm::vec3 MinBB;
	glm::vec3 MaxBB;
	glm::mat3 ModelInvTrans;
};

struct geometry_create_info_t
{
	geometry_interleaved_info_t Interleaved;
	glm::mat4 Model;
};

// User accessible
struct geometry_state_t
{
	uint8_t Position;
	uint8_t Visible[PROGRAM_MAX_OBJECTS];

	geometry_interleaved_info_t Interleaved[PROGRAM_MAX_OBJECTS];
	glm::mat4 Model[PROGRAM_MAX_OBJECTS];

	void Alloc(const geometry_create_info_t& CreateInfo);
	void Free(uint8_t FreedIndex);

	private:

	// Prevent Push() or Pop() being called outside of provided Alloc(), Free() functions
	free_list_t FreeList;
};

#endif

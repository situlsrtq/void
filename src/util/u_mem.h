#ifndef MBOX_UMEMORY_H
#define MBOX_UMEMORY_H


#include <stdint.h>
#include <stdio.h>

#include "u_math.h"


#define PROGRAM_MAX_OBJECTS 64
#define VIS_STATUS_VISIBLE 1
#define VIS_STATUS_INVISIBLE 0
#define VIS_STATUS_FREED 2


// For internal use by geometry_state_t, never user-accessible
struct free_list_t
{
	uint8_t NextFreePosition = 0;
	uint8_t OpenPositions[PROGRAM_MAX_OBJECTS];

	void Push(uint8_t Index);
	uint8_t Pop();
};


// User accessible
struct geometry_create_info_t
{
	bool New;
	bool Deleted;
	uint32_t IndexType;
	uint32_t IndexCount;
	uint64_t VAttrCount;
	uint64_t ByteOffsetEBO;
	uint64_t OffsetVBO;
	uMATH::vec3f_t Color;
	uMATH::vec3f_t MinBB;
	uMATH::vec3f_t MaxBB;
	uMATH::mat4f_t Model;
};


//User accessible
struct geometry_state_t
{
	uint8_t Position;
	uint8_t Visible[PROGRAM_MAX_OBJECTS];

	uint32_t IndexType[PROGRAM_MAX_OBJECTS];
	uint32_t IndexCount[PROGRAM_MAX_OBJECTS];
	uint64_t VAttrCount[PROGRAM_MAX_OBJECTS];
	uint64_t ByteOffsetEBO[PROGRAM_MAX_OBJECTS];
	uint64_t OffsetVBO[PROGRAM_MAX_OBJECTS];
	uMATH::vec3f_t Color[PROGRAM_MAX_OBJECTS];
	uMATH::vec3f_t MinBB[PROGRAM_MAX_OBJECTS];
	uMATH::vec3f_t MaxBB[PROGRAM_MAX_OBJECTS];
	uMATH::mat4f_t Model[PROGRAM_MAX_OBJECTS];

	void Alloc(const geometry_create_info_t &CreateInfo);
	void Free(uint8_t FreedIndex);
	
	private:
	
	// Prevent Push() or Pop() being called outside of provided Alloc(), Free() functions
	free_list_t FreeList;
};


#endif

#ifndef MBOX_UMEMORY_H
#define MBOX_UMEMORY_H


#include <stdint.h>
#include <stdio.h>

#include "u_math.h"


#define PROGRAM_MAX_OBJECTS 64
#define VIS_STATUS_VISIBLE 1
#define VIS_STATUS_INVISIBLE 0
#define VIS_STATUS_FREED 2


struct free_list_t
{
	uint8_t NextFreePosition = 0;
	uint8_t OpenPositions[PROGRAM_MAX_OBJECTS];

	void Push(uint8_t Index);
	uint8_t Pop();
};


struct geometry_create_info_t
{
	bool New;
	bool Deleted;
	float Intensity;
	float Scale;
	float RotationAngle;
	uMATH::vec3f_t RotationAxis;
	uMATH::vec3f_t Position;
	uMATH::vec3f_t Color;
	uMATH::mat4f_t Model;

	void DecomposeModelM4();
	void ComposeModelM4();
};


struct geometry_state_t
{
	uint8_t Visible[PROGRAM_MAX_OBJECTS];
	float Scale[PROGRAM_MAX_OBJECTS];
	float Intensity[PROGRAM_MAX_OBJECTS];

	uMATH::vec3f_t Color[PROGRAM_MAX_OBJECTS];
	uMATH::mat4f_t Model[PROGRAM_MAX_OBJECTS];

	uint8_t Position;

	void Alloc();
	void Alloc(const geometry_create_info_t &CreateInfo);
	void Free(uint8_t FreedIndex);
	
	private:
	
	// Prevent Push() or Pop() being called outside of provided Alloc(), Free() functions
	free_list_t FreeList;
};


#endif
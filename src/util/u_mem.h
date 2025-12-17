#ifndef VOID_UMEMORY_H
#define VOID_UMEMORY_H

#include "u_types.h"
#include "u_util.h"

struct linear_arena_t
{
	uint32_t Position;
	uint32_t Size;
	void* BaseAddr;

	int Init(uint32_t size);
	int Alloc(uint32_t* Result, uint32_t len);
	void Reset();
	void Release();

	private:

	int Resize();
};

struct index_free_list_t
{
	uint32_t NextFreePosition = 0;
	uint32_t OpenPositions[PROGRAM_MAX_OBJECTS];

	void Push(uint32_t Index);
	uint32_t Pop();
};

struct linked_block_t
{
	linked_block_t* prev;
	linked_block_t* next;
	uint32_t base_index;
	uint32_t size;
};

struct block_free_list_t
{
	linked_block_t* root;

	void Push(uint32_t base_index, uint32_t size);
	uint32_t Pop(uint32_t req_size);
	void Merge(linked_block_t* node);
};

#endif

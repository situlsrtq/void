#ifndef VOID_UMEMORY_H
#define VOID_UMEMORY_H

#include <math.h>

#include "u_types.h"
#include "u_util.h"

struct linear_arena_t
{
	u64 position;
	u64 size;
	u64 step_size;
	void* base_addr;

	int init(u64 size, u64 step_size);
	void reset();
	void release();
};

struct alloc_params_t
{
	u64* handle;
	u64 len;
};

typedef int (*alloc_func)(linear_arena_t*, u64*, u64);

struct lin_arena_info_t
{
	linear_arena_t* arena;
};

int arena_alloc(linear_arena_t* arena, u64* handle, u64 len);
void add_to_arena(linear_arena_t* arena, lin_arena_info_t* info);

struct index_free_list_t
{
	u32 NextFreePosition = 0;
	u32 OpenPositions[PROGRAM_MAX_OBJECTS];

	void Push(u32 Index);
	u32 Pop();
};

struct linked_block_t
{
	linked_block_t* prev;
	linked_block_t* next;
	u32 base_index;
	u32 size;
};

struct block_free_list_t
{
	linked_block_t* root;

	void Push(u32 base_index, u32 size);
	u32 Pop(u32 req_size);
	void Merge(linked_block_t* node);
};

#endif

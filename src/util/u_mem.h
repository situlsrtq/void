#ifndef VOID_UMEMORY_H
#define VOID_UMEMORY_H

#include <math.h>

#include "u_types.h"
#include "u_util.h"

/// <summary>
/// Fixed-size linear arena. Grows only, does not shrink
/// </summary>
struct linear_arena_t
{
	u64 position;
	u64 size;
	void* base_addr;

	int init(u64 size);
	void reset();
	void release();
};

int arena_alloc(linear_arena_t* arena, u64* handle, u64 len);
void* pointer_from_arena(linear_arena_t* arena, u64 offset);

/// <summary>
/// Fixed-size freelist allocator.
/// This structure is used to manage a data array,
/// it does not provide one
/// </summary>
struct index_free_list_t
{
	u32 next_free_pos = 0;
	u32 open_positions[PROGRAM_MAX_OBJECTS];

	void push(u32 Index);
	u32 pop();
};

struct linked_block_t
{
	linked_block_t* prev;
	linked_block_t* next;
	u32 base_index;
	u32 size;
};

/// <summary>
/// Dynamically sized freelist block allocator.
/// This structure is used to manage a data array,
/// it does not provide one
/// </summary>
struct block_free_list_t
{
	linked_block_t* root;

	void push(u32 base_index, u32 size);
	u32 pop(u32 req_size);

	private: 

	void merge(linked_block_t* node);
};

#endif

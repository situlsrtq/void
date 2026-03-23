#ifndef VOID_UMEMORY_H
#define VOID_UMEMORY_H

#include <math.h>

#include "u_types.h"
#include "u_util.h"

/// <summary>
/// Dynamically sized linear arena. Grows only, does not shrink
/// </summary>
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

struct lin_arena_info_t
{
	linear_arena_t* arena;
};

int arena_alloc(linear_arena_t* arena, u64* handle, u64 len);
void add_to_arena(linear_arena_t* arena, lin_arena_info_t* info);

/// <summary>
/// Takes an offset-based handle and returns a pointer to memory in a linear arena.
/// WARN: These arenas are resizable, do not store the returned pointers.
/// </summary>
/// <param name="info"></param>
/// <param name="offset"></param>
/// <returns></returns>
void* pointer_from_arena(lin_arena_info_t* info, u64 offset);

/// <summary>
/// Fixed-size freelist allocator
/// </summary>
struct index_free_list_t
{
	u32 next_free_pos = 0;
	u32 open_positions[PROGRAM_MAX_OBJECTS];

	void push(u32 Index);
	u32 pop();
};

/// <summary>
/// Dynamically sized block allocator
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

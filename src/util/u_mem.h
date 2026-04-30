#ifndef VOID_UMEMORY_H
#define VOID_UMEMORY_H

#include <math.h>

#include "PAL.h"
#include "u_types.h"
#include "u_util.h"

/// <summary>
/// Fixed-size linear arena. Grows only, does not shrink
/// </summary>
struct linear_arena_t
{
	size_t position;
	size_t size;
	void* base_addr;

	int init(size_t size);
	void reset();
	void release();
};

int arena_alloc(linear_arena_t* arena, size_t* handle, size_t len);
void* pointer_from_arena(linear_arena_t* arena, size_t offset);

/// <summary>
/// Fixed-size freelist allocator.
/// This structure is used to manage a data array,
/// it does not provide one
/// </summary>
struct index_free_list_t
{
	size_t base_array_pos;
	size_t next_free_pos;
	size_t open_positions[PROGRAM_MAX_OBJECTS];

	void push(size_t Index);
	size_t pop();
};

struct linked_block_t
{
	linked_block_t* prev;
	linked_block_t* next;
	size_t base_index;
	size_t size;
};

/// <summary>
/// Dynamically sized freelist block allocator.
/// This structure is used to manage a data array,
/// it does not provide one
/// </summary>
struct block_free_list_t
{
	linked_block_t* root;
	size_t base_array_pos;

	void push(size_t base_index, size_t size);
	size_t pop(size_t req_size);

	private: 

	void merge(linked_block_t* node);
};

#endif

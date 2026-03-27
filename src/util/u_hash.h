#ifndef VOID_HASHMAP_H
#define VOID_HASHMAP_H

#include <murmur/MurmurHash3.h>

#include "u_mem.h"
#include "u_types.h"

#define TABLE_SIZE PROGRAM_MAX_OBJECTS
#define KEY_NOT_FOUND OBJECT_ALLOC_ERROR
#define TABLE_SEED 42069

#define FLAG_AVAILABLE 0
#define FLAG_POPULATED 1

struct robin_node_t
{
	u64 handle;
	u32 value;
	u32 displacement;
	u8 flag;
};

struct hash_table_t
{
	linear_arena_t* string_arena;
	robin_node_t table[PROGRAM_MAX_OBJECTS];
	u32 probes_before_boundary;
	u32 occupancy;

	int init(linear_arena_t* string_arena);
};

void rh_hash_insert(hash_table_t* table, void* key, int len, u32 value);
void rh_hash_remove(hash_table_t* table, void* key);
u32 rh_hash_find(hash_table_t* table, void* search_key, int len);

#endif

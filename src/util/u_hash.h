#ifndef VOID_HASHMAP_H
#define VOID_HASHMAP_H

#include <stdint.h>

#include<murmur/MurmurHash3.h>

#define TABLE_SIZE 512
#define TABLE_SEED 42069

#define FLAG_POPULATED 1

struct robin_node_t
{
	int8_t flag;
	uint32_t value;
	uint32_t displacement;
};

struct hash_table_t
{
	robin_node_t table[TABLE_SIZE];

	void Insert(void* key, int len, int32_t* value);
	void Remove(void* key);
	void Find(void* key);
};

#endif

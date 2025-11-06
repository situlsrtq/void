#ifndef VOID_HASHMAP_H
#define VOID_HASHMAP_H

#include <stdint.h>

#include<murmur/MurmurHash3.h>

#define TABLE_SIZE 512
#define KEY_NOT_FOUND TABLE_SIZE+1
#define TABLE_SEED 42069

#define FLAG_AVAILABLE 0
#define FLAG_POPULATED 1

struct robin_node_t
{
	void* key;
	uint32_t value;
	uint32_t displacement;
	uint8_t flag;
};

struct hash_table_t
{
	robin_node_t table[TABLE_SIZE];

	void Insert(void* key, int len, uint32_t value);
	void Remove(void* key);
	uint32_t Find(void* search_key, int len);

	private:

	uint32_t probes_before_boundary;
	void Move(robin_node_t res, robin_node_t* frame);
};

extern hash_table_t* g_test_table;

#endif

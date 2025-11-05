#include "u_hash.h"

void hash_table_t::Move(robin_node_t res, robin_node_t* frame)
{
	if(frame->flag == FLAG_AVAILABLE)
	{
		frame->key = res.key;	
		frame->value = res.value;	
		frame->displacement = res.displacement;	
		frame->flag = FLAG_POPULATED;	
		return;
	}

	frame++;
	res.displacement++;

	while(frame->displacement > res.displacement)
	{
		frame++;
		res.displacement++;
	}

	if(frame->flag != FLAG_AVAILABLE)
	{
		robin_node_t temp = *frame;
		frame->key = res.key;	
		frame->value = res.value;	
		frame->displacement = res.displacement;	
		frame->flag = FLAG_POPULATED;	
		Move(temp, frame++);
		return;
	}
}

void hash_table_t::Insert(void* key, int len, uint32_t value)
{
	uint32_t hashed_key;
	MurmurHash3_x86_32(key, len, TABLE_SEED, &hashed_key);
	hashed_key %= TABLE_SIZE;

	robin_node_t res = {key, value, 0, 0};
	robin_node_t* frame = &table[hashed_key];

	Move(res, frame);
}

uint32_t hash_table_t::Find(void* search_key, int len)
{
	uint32_t hashed_key;
	MurmurHash3_x86_32(search_key, len, TABLE_SEED, &hashed_key);
	hashed_key %= TABLE_SIZE;

	uint32_t displacement = 0;
	robin_node_t* frame = &table[hashed_key];

	while(displacement <= frame->displacement)
	{
		if(frame->key == search_key)
		{
			return frame->value; 
		}

		frame++;
		displacement++;
	}

	return KEY_NOT_FOUND;
}

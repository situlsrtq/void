#include "u_hash.h"

void hash_table_t::Move(robin_node_t res, robin_node_t* frame, uint32_t probes_before_boundary)
{
	if (probes_before_boundary == 0)
	{
		frame = &table[0];
	}

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
	probes_before_boundary--;
	if (probes_before_boundary == 0)
	{
		frame = &table[0];
	}

	while(frame->displacement > res.displacement)
	{
		frame++;
		res.displacement++;
		probes_before_boundary--;
		if(probes_before_boundary == 0)
		{
			frame = &table[0];
		}
	}

	if(frame->flag != FLAG_AVAILABLE)
	{
		robin_node_t temp = *frame;
		frame->key = res.key;	
		frame->value = res.value;	
		frame->displacement = res.displacement;	
		frame->flag = FLAG_POPULATED;	
		Move(temp, frame++, probes_before_boundary--);
		return;
	}
}

void hash_table_t::Insert(void* key, int len, uint32_t value)
{
	uint32_t hashed_key;
	MurmurHash3_x86_32(key, len, TABLE_SEED, &hashed_key);
	hashed_key %= TABLE_SIZE;
	uint32_t probes_before_boundary = TABLE_SIZE - hashed_key;

	robin_node_t res = {key, value, 0, 0};
	robin_node_t* frame = &table[hashed_key];

	Move(res, frame, probes_before_boundary);
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

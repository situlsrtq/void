#include "u_hash.h"

hash_table_t* g_test_table = 0x0;

void hash_table_t::Move(robin_node_t res, robin_node_t* frame)
{
	if(probes_before_boundary == 0)
	{
		frame = &table[0];
		probes_before_boundary = TABLE_SIZE;
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
	if(probes_before_boundary == 0)
	{
		frame = &table[0];
		probes_before_boundary = TABLE_SIZE;
	}

	while(frame->displacement > res.displacement)
	{
		frame++;
		res.displacement++;
		probes_before_boundary--;
		if(probes_before_boundary == 0)
		{
			frame = &table[0];
			probes_before_boundary = TABLE_SIZE;
		}
	}

	if(frame->flag != FLAG_AVAILABLE)
	{
		robin_node_t temp = *frame;
		frame->key = res.key;
		frame->value = res.value;
		frame->displacement = res.displacement;
		frame->flag = FLAG_POPULATED;
		probes_before_boundary--;
		Move(temp, frame++);
		return;
	}
}

uint32_t hash_table_t::Insert(void* key, int len, uint32_t value)
{
	uint32_t hash_key;
	MurmurHash3_x86_32(key, len, TABLE_SEED, &hash_key);
	hash_key %= TABLE_SIZE;

	robin_node_t res = {key, value, 0, 0};
	robin_node_t* frame = &table[hash_key];
	probes_before_boundary = TABLE_SIZE - hash_key;

	uint32_t steps = 0;
	while(steps < TABLE_SIZE)
	{
		if(probes_before_boundary == 0)
		{
			frame = &table[0];
			hash_key = 0;
			probes_before_boundary = TABLE_SIZE-1;
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
		if(probes_before_boundary == 0)
		{
			frame = &table[0];
			probes_before_boundary = TABLE_SIZE;
		}

		while(frame->displacement > res.displacement)
		{
			frame++;
			res.displacement++;
			probes_before_boundary--;
			if(probes_before_boundary == 0)
			{
				frame = &table[0];
				probes_before_boundary = TABLE_SIZE;
			}
		}

		if(frame->flag != FLAG_AVAILABLE)
		{
			robin_node_t temp = *frame;
			frame->key = res.key;
			frame->value = res.value;
			frame->displacement = res.displacement;
			frame->flag = FLAG_POPULATED;
			probes_before_boundary--;
			Move(temp, frame++);
			return;
		}
	}
}

uint32_t hash_table_t::Find(void* search_key, int len)
{
	uint32_t hashed_key;
	MurmurHash3_x86_32(search_key, len, TABLE_SEED, &hashed_key);
	hashed_key %= TABLE_SIZE;

	probes_before_boundary = TABLE_SIZE - hashed_key;
	uint32_t displacement = 0;
	robin_node_t* frame = &table[hashed_key];

	while(displacement <= frame->displacement)
	{
		if(frame->key == 0x0)
		{
			frame++;
			displacement++;
			continue;
		}

		if(!strcmp((char*)frame->key, (char*)search_key))
		{
			return frame->value;
		}

		frame++;
		displacement++;
		probes_before_boundary--;
		if(probes_before_boundary == 0)
		{
			frame = &table[0];
			probes_before_boundary = TABLE_SIZE;
		}
	}

	return KEY_NOT_FOUND;
}

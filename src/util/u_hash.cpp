#include "u_hash.h"

hash_table_t* g_test_table = 0x0;

void hash_table_t::Insert(void* key, int len, uint32_t value)
{
	if(occupancy == TABLE_SIZE)
	{
		printf("System: could not insert (hash table full)\n");
		return;
	}

	robin_node_t res = {0, value, 0, 0};
	int string_res = StringArena.Alloc(&res.offset, len);
	if(string_res == EXIT_FAILURE)
	{
		printf("System: could not insert (string alloc failure)\n");
		return;
	}
	memcpy((uint8_t*)StringArena.BaseAddr + res.offset, key, len);

	uint32_t hash_key;
	MurmurHash3_x86_32(key, len, TABLE_SEED, &hash_key);
	hash_key %= TABLE_SIZE;

	robin_node_t* frame = &table[hash_key];
	probes_before_boundary = TABLE_SIZE - hash_key;

	bool frame_needs_placing = true;
	while(frame_needs_placing)
	{
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

		if(frame->flag == FLAG_AVAILABLE)
		{
			frame->offset = res.offset;
			frame->value = res.value;
			frame->displacement = res.displacement;
			frame->flag = FLAG_POPULATED;
			frame_needs_placing = false;
		}
		else
		{
			robin_node_t temp = *frame;
			frame->offset = res.offset;
			frame->value = res.value;
			frame->displacement = res.displacement;
			frame->flag = FLAG_POPULATED;

			res = temp;
			// Increment here to prevent the bumped frame from looping right back around and
			// bumping the frame that was just placed (in the case where the displacements are equal)
			frame++;
			res.displacement++;
			probes_before_boundary--;
			if(probes_before_boundary == 0)
			{
				frame = &table[0];
				probes_before_boundary = TABLE_SIZE;
			}
		}
	}

	occupancy++;
	return;
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
		if(!strcmp((char*)StringArena.BaseAddr + frame->offset, (char*)search_key))
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

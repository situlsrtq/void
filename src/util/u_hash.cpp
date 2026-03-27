#include "u_hash.h"

hash_table_t* g_test_table = 0x0;

int hash_table_t::init(linear_arena_t* arena)
{
	if(arena == 0x0)
	{
		printf("Hash Table: invalid memory arena provided\n");
		return EXIT_FAILURE;
	}
	string_arena = arena;
	probes_before_boundary = 0;
	occupancy = 0;
	return EXIT_SUCCESS;
}

void rh_hash_insert(hash_table_t* table, void* key, int len, u32 value)
{
	if(table->occupancy == TABLE_SIZE)
	{
		printf("System: could not insert (hash table full)\n");
		return;
	}

	robin_node_t res = {0x0, value, 0, 0};
	int success = arena_alloc(table->string_arena, &res.handle, len);
	if(success == EXIT_FAILURE)
	{
		printf("System: could not insert (string alloc failure)\n");
		return;
	}
	void* res_address = pointer_from_arena(table->string_arena, res.handle);
	memcpy(res_address, key, len);

	u32 hash_key;
	MurmurHash3_x86_32(key, len, TABLE_SEED, &hash_key);
	hash_key %= TABLE_SIZE;

	robin_node_t* frame = &table->table[hash_key];
	table->probes_before_boundary = TABLE_SIZE - hash_key;

	bool frame_needs_placing = true;
	while(frame_needs_placing)
	{
		while(frame->displacement > res.displacement)
		{
			frame++;
			res.displacement++;
			table->probes_before_boundary--;
			if(table->probes_before_boundary == 0)
			{
				frame = &table->table[0];
				table->probes_before_boundary = TABLE_SIZE;
			}
		}

		if(frame->flag == FLAG_AVAILABLE)
		{
			frame->handle = res.handle;
			frame->value = res.value;
			frame->displacement = res.displacement;
			frame->flag = FLAG_POPULATED;
			frame_needs_placing = false;
		}
		else
		{
			robin_node_t temp = *frame;
			frame->handle = res.handle;
			frame->value = res.value;
			frame->displacement = res.displacement;
			frame->flag = FLAG_POPULATED;

			res = temp;
			// Increment here to prevent the bumped frame from looping right back around and
			// bumping the frame that was just placed (in the case where the displacements are equal)
			frame++;
			res.displacement++;
			table->probes_before_boundary--;
			if(table->probes_before_boundary == 0)
			{
				frame = &table->table[0];
				table->probes_before_boundary = TABLE_SIZE;
			}
		}
	}

	table->occupancy++;
	return;
}

u32 rh_hash_find(hash_table_t* table, void* search_key, int len)
{
	u32 hashed_key;
	MurmurHash3_x86_32(search_key, len, TABLE_SEED, &hashed_key);
	hashed_key %= TABLE_SIZE;

	table->probes_before_boundary = TABLE_SIZE - hashed_key;
	u32 displacement = 0;
	robin_node_t* frame = &table->table[hashed_key];

	while(displacement <= frame->displacement)
	{
		char *frame_string = (char*)pointer_from_arena(table->string_arena, frame->handle);
		if(!strcmp(frame_string, (char*)search_key))
		{
			return frame->value;
		}

		frame++;
		displacement++;
		table->probes_before_boundary--;
		if(table->probes_before_boundary == 0)
		{
			frame = &table->table[0];
			table->probes_before_boundary = TABLE_SIZE;
		}
	}

	return KEY_NOT_FOUND;
}

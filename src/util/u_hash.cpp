#include "u_hash.h"

void hash_table_t::Insert(void* key, int len, int32_t* value)
{
	uint32_t hashed_key;
	MurmurHash3_x86_32(key, len, TABLE_SEED, &hashed_key);
	hashed_key %= TABLE_SIZE;

	if(table[hashed_key].flag == 0)
	{
		table[hashed_key].value = *value;	
		table[hashed_key].flag = FLAG_POPULATED;	
	}
}

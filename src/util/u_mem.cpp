#include "u_mem.h"

int linear_arena_t::init(u64 size)
{
	base_addr = malloc(size);
	if(base_addr == 0x0)
	{
		printf("System: failed to allocate arena\n");
		return EXIT_FAILURE;
	}

	position = 0;
	this->size = size;

	return EXIT_SUCCESS;
}

void linear_arena_t::reset()
{
	position = 0;
	return;
}

void linear_arena_t::release()
{
	UTIL::Free(base_addr);
	position = 0;
	size = 0;
	return;
}

int arena_alloc(linear_arena_t* arena, u64* handle, u64 len)
{
	if(arena->position + len > arena->size)
	{
		printf("Arena: could not allocate (resize failed)\n");
		return EXIT_FAILURE;
	}

	*handle = arena->position;
	arena->position += len;
	return EXIT_SUCCESS;
}

void* pointer_from_arena(linear_arena_t* arena, u64 offset)
{
	void* res = (u8*)arena->base_addr + offset;
	return res;
}

u32 index_free_list_t::pop()
{
	next_free_pos--;

	u32 res = open_positions[next_free_pos];
	return res;
}

void index_free_list_t::push(u32 FreedIndex)
{
	if(next_free_pos == PROGRAM_MAX_OBJECTS)
	{
		printf("FreeList: Object Limit Reached\n");
		return;
	}

	open_positions[next_free_pos] = FreedIndex;
	next_free_pos++;
}

struct linked_block_t
{
	linked_block_t* prev;
	linked_block_t* next;
	u32 base_index;
	u32 size;
};

u32 block_free_list_t::pop(u32 req_size)
{
	u32 res = root->base_index;

	root->base_index += req_size;
	root->size -= req_size;

	linked_block_t* node = root;
	linked_block_t* temp = root;
	while((node->next != 0x0) && (node->size < node->next->size))
	{
		temp = node->next;
		node->next = temp->next;
		node->next->prev = node;
		temp->prev = node->prev;
		temp->prev->next = temp;
		node->prev = temp;
		temp->next = node;
	}

	root = temp;

	return res;
}

void block_free_list_t::push(u32 base_index, u32 size)
{
	if(root == 0x0)
	{
		root = (linked_block_t*)UTIL::Malloc(sizeof(linked_block_t));
		root->prev = 0x0;
		root->next = 0x0;
		root->base_index = base_index;
		root->size = size;
		return;
	}

	linked_block_t* node = root;
	while(node->size > size)
	{
		linked_block_t* temp = node;
		node = node->next;
		if(node == 0x0)
		{
			node = (linked_block_t*)UTIL::Malloc(sizeof(linked_block_t));
			temp->next = node;
			node->prev = temp;
			node->next = 0x0;
			node->base_index = base_index;
			node->size = size;
			merge(node);
			return;
		}
	}

	linked_block_t* new_node = (linked_block_t*)UTIL::Malloc(sizeof(linked_block_t));
	new_node->next = node;
	new_node->prev = node->prev;
	node->prev->next = new_node;
	node->prev = new_node;
	new_node->base_index = base_index;
	new_node->size = size;
	merge(new_node);
	return;
}

void block_free_list_t::merge(linked_block_t* node)
{
	if((node->next != 0x0) && (node->next->base_index == (node->base_index + node->size)))
	{
		node->size += node->next->size;
		if(node->next->next != 0x0)
		{
			linked_block_t* temp = node->next->next;
			temp->prev = node;
			UTIL::Free(node->next);
			node->next = temp;
		}
		else
		{
			UTIL::Free(node->next);
		}
	}

	if((node->prev != 0x0) && ((node->prev->base_index + node->prev->size) == node->base_index))
	{
		linked_block_t* temp = node;
		node = node->prev;
		node->size += temp->size;
		node->next = temp->next;
		UTIL::Free(temp);
	}

	while((node->prev != 0x0) && (node->size > node->prev->size))
	{
		linked_block_t* temp = node->prev;
		node->prev = temp->prev;
		temp->next = node->next;
		node->next = temp;
		temp->prev = node;
	}

	return;
}

#include "u_mem.h"

int linear_arena_t::init(u64 size, u64 step_size)
{
	base_addr = malloc(size);
	if(base_addr == 0x0)
	{
		printf("System: failed to allocate arena\n");
		return EXIT_FAILURE;
	}

	position = 0;
	this->size = size;
	this->step_size = step_size;

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

// Block allocator will only ever size up
int resize_arena(linear_arena_t* arena, u64 overflow)
{
	u32 num_steps = ceil(overflow / arena->step_size);

	void* tmp = realloc(arena->base_addr, arena->size + (arena->step_size * num_steps));
	if(tmp == 0x0)
	{
		printf("System: failed to reallocate arena on resize request\n");
		return EXIT_FAILURE;
	}

	arena->base_addr = tmp;
	arena->size += arena->step_size;
	return EXIT_SUCCESS;
}

int arena_alloc(linear_arena_t* arena, u64* handle, u64 len)
{
	if(arena->position + len > arena->size)
	{
		int res = resize_arena(arena, arena->position + len - arena->size);
		if(res == EXIT_FAILURE)
		{
			printf("Arena: could not allocate (resize failed)\n");
			return EXIT_FAILURE;
		}
	}

	*handle = arena->position;
	arena->position += len;
	return EXIT_SUCCESS;
}

void add_to_arena(linear_arena_t *arena, lin_arena_info_t *info)
{
	info->arena = arena;
}

u32 index_free_list_t::Pop()
{
	NextFreePosition--;

	u32 res = OpenPositions[NextFreePosition];
	return res;
}

void index_free_list_t::Push(u32 FreedIndex)
{
	if(NextFreePosition == PROGRAM_MAX_OBJECTS)
	{
		printf("FreeList: Object Limit Reached\n");
		return;
	}

	OpenPositions[NextFreePosition] = FreedIndex;
	NextFreePosition++;
}

u32 block_free_list_t::Pop(u32 req_size)
{
	u32 res = root->base_index;

	root->base_index += req_size;
	root->size -= req_size;

	linked_block_t* node = root;
	while(node->size < node->next->size)
	{
		linked_block_t* temp = node->next;
		node->next = temp->next;
		temp->prev = node->prev;
		node->prev = temp;
		temp->next = node;
	}

	return res;
}

void block_free_list_t::Push(u32 base_index, u32 size)
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
			node->prev = temp;
			node->next = 0x0;
			node->base_index = base_index;
			node->size = size;
			Merge(node);
			return;
		}
	}

	linked_block_t* new_node = (linked_block_t*)UTIL::Malloc(sizeof(linked_block_t));
	new_node->next = node;
	new_node->prev = node->prev;
	node->prev = new_node;
	new_node->base_index = base_index;
	new_node->size = size;
	Merge(new_node);
	return;
}

void block_free_list_t::Merge(linked_block_t* node)
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

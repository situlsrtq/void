#include "u_mem.h"

uint32_t index_free_list_t::Pop()
{
	NextFreePosition--;

	uint32_t res = OpenPositions[NextFreePosition];
	return res;
}

void index_free_list_t::Push(uint32_t FreedIndex)
{
	if(NextFreePosition == PROGRAM_MAX_OBJECTS)
	{
		printf("FreeList: Object Limit Reached\n");
		return;
	}

	OpenPositions[NextFreePosition] = FreedIndex;
	NextFreePosition++;
}

uint32_t block_free_list_t::Pop(uint32_t req_size)
{
	uint32_t res = root->base_index;

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

void block_free_list_t::Push(uint32_t base_index, uint32_t size)
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

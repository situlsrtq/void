#include "u_mem.h"

uint32_t free_list_t::Pop()
{
	NextFreePosition--;

	uint32_t res = OpenPositions[NextFreePosition];
	return res;
}

void free_list_t::Push(uint32_t FreedIndex)
{
	if(NextFreePosition == PROGRAM_MAX_OBJECTS)
	{
		printf("FreeList: Object Limit Reached\n");
		return;
	}

	OpenPositions[NextFreePosition] = FreedIndex;
	NextFreePosition++;
}

uint32_t geometry_state_t::Alloc(const geometry_create_info_t& CreateInfo)
{
	uint32_t index;

	if(FreeList.NextFreePosition > 0)
	{
		index = FreeList.Pop();
	}
	else
	{
		index = Position;

		if(index >= PROGRAM_MAX_OBJECTS)
		{
			printf("System: Object Limit Reached\n");
			return OBJECT_ALLOC_ERROR;
		}

		Position++;
	}

	Visible[index] = VIS_STATUS_VISIBLE;
	Interleaved[index] = CreateInfo.Interleaved;
	Model[index] = CreateInfo.Model;

	return index;
}

void geometry_state_t::Free(uint32_t FreedIndex)
{
	if(Position == 0)
	{
		printf("System: Object array empty, nothing to free\n");
		return;
	}
	if(FreedIndex >= Position || FreedIndex >= PROGRAM_MAX_OBJECTS)
	{
		printf("System: Out of bounds on free list\n");
		return;
	}

	FreeList.Push(FreedIndex);
	Visible[FreedIndex] = VIS_STATUS_FREED;
}

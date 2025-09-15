#include "u_mem.h"


uint8_t free_list_t::Pop()
{
	NextFreePosition--;

	uint8_t res = OpenPositions[NextFreePosition];
	return res;
}


void free_list_t::Push(uint8_t FreedIndex)
{
	if(NextFreePosition == PROGRAM_MAX_OBJECTS)
	{
		printf("FreeList: Object Limit Reached\n");
		return;
	}

	OpenPositions[NextFreePosition] = FreedIndex;
	NextFreePosition++;
}


void geometry_state_t::Alloc(const geometry_create_info_t &CreateInfo)
{
	uint8_t index;

	if(FreeList.NextFreePosition > 0)
	{
		index = FreeList.Pop();
	}
	else
	{
		index = Position;

		if (index >= PROGRAM_MAX_OBJECTS)
		{
			printf("System: Object Limit Reached\n");
			return;
		}

		Position++;
	}

	Visible[index] = VIS_STATUS_VISIBLE;
	IndexType[index] = CreateInfo.IndexType;
	IndexCount[index] = CreateInfo.IndexCount;
	VAttrStride[index] = CreateInfo.VAttrStride;
	VAttrCount[index] = CreateInfo.VAttrCount;
	IndexBaseAddr[index] = CreateInfo.IndexBaseAddr;
	VAttrBaseAddr[index] = CreateInfo.VAttrBaseAddr;
	Color[index] = CreateInfo.Color;
	MinBB[index] = CreateInfo.MinBB;
	MaxBB[index] = CreateInfo.MaxBB;
	Model[index] = CreateInfo.Model;
}


void geometry_state_t::Free(uint8_t FreedIndex)
{
	if (Position == 0)
	{
		printf("System: Object array empty, nothing to free\n");
		return;
	}
	if (FreedIndex >= Position || FreedIndex >= PROGRAM_MAX_OBJECTS)
	{
		printf("System: Out of bounds on free list\n");
		return;
	}

	FreeList.Push(FreedIndex);
	Visible[FreedIndex] = VIS_STATUS_FREED;
}

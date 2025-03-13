#include "u_mem.h"


uint8_t free_list_t::Pop()
{
	uint8_t res = OpenPositions[NextFreePosition];

	if(NextFreePosition > 0)
	{
		NextFreePosition--;
	}

	NumFree--;
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
	NumFree++;
}


void geometry_state_t::Alloc()
{
	uint8_t index;

	if(FreeList.NumFree > 0)
	{
		index = FreeList.Pop();
	}
	else
	{
		index = Position;
		Position++;
	}

	if(index == PROGRAM_MAX_OBJECTS)
	{
		printf("System: Object Limit Reached\n");
		return;
	}

	Visible[index] = 1;
	Scale[index] = 1.0f;
	Intensity[index] = 0.5f;
	Color[index] = { 1.0f, 0.5f, 0.31f };
	SetTransform(&Model[index]);
}


void geometry_state_t::Alloc(const geometry_create_info_t &CreateInfo)
{
	uint8_t index;

	if(FreeList.NumFree > 0)
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
	Scale[index] = CreateInfo.Scale;
	Intensity[index] = CreateInfo.Intensity;
	Color[index] = CreateInfo.Color;
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

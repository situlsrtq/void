#include "u_mem.h"


void geometry_create_info_t::DecomposeModelM4()
{
	Position = { Model.m[0][3], Model.m[1][3], Model.m[2][3] };

	uMATH::vec3f_t axis = {Model.m[0][0], Model.m[0][1], Model.m[0][2]};
	// We don't support non-uniform scaling, so we only need to check one axis to get the object's scale
	Scale = sqrtf((axis.x * axis.x) + (axis.y * axis.y) + (axis.z * axis.z));
	
	Rotation.x = Model.m[0][0] / Scale;
	Rotation.y = Model.m[1][0] / Scale;
	Rotation.z = Model.m[2][0] / Scale;
}


void geometry_create_info_t::ComposeModelM4()
{
	uMATH::SetTransform(&Model);
	uMATH::Scale(&Model, Scale);
	uMATH::MatrixRotate(&Model, 40.0f, Rotation);
	uMATH::Translate(&Model, Position);
}


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


void geometry_state_t::Alloc()
{
	uint8_t index;

	if(FreeList.NextFreePosition > 0)
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

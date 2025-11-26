#include "scene.h"

uint32_t scene_info_t::AddMesh(uint32_t num_primitives)
{
	uint32_t index;

	if(MeshList.NextFreePosition > 0)
	{
		index = MeshList.Pop();
	}
	else
	{
		index = MeshPosition;

		if(index >= PROGRAM_MAX_OBJECTS)
		{
			printf("System: Object Limit Reached\n");
			return OBJECT_ALLOC_ERROR;
		}

		MeshPosition++;
	}

	if(PrimList.root == 0x0 || (PrimList.root->size < num_primitives))
	{
		if((PrimPosition + num_primitives) > PROGRAM_MAX_OBJECTS)
		{
			printf("System: Object Limit Reached\n");
			return OBJECT_ALLOC_ERROR;
		}

		Mesh[index].base_index = PrimPosition;
		Mesh[index].size = num_primitives;
		PrimPosition += num_primitives;
		return index;
	}

	Mesh[index].base_index = PrimList.Pop(num_primitives);
	Mesh[index].size = num_primitives;
	return index;
}

void scene_info_t::AddPrimitive(const primitive_create_info_t& CreateInfo, uint32_t index)
{
	Prim[index] = CreateInfo;
	return;
}

uint32_t scene_info_t::AddNode(const node_create_info_t& CreateInfo, const glm::mat4& ModelIn)
{
	uint32_t index;

	if(NodeList.NextFreePosition > 0)
	{
		index = NodeList.Pop();
	}
	else
	{
		index = NodePosition;

		if(index >= PROGRAM_MAX_OBJECTS)
		{
			printf("System: Object Limit Reached\n");
			return OBJECT_ALLOC_ERROR;
		}

		NodePosition++;
	}

	Node[index] = CreateInfo;
	ModelMatrix[index] = ModelIn;

	return index;
}

void scene_info_t::FreeNode(uint32_t FreedIndex)
{
	if(NodePosition == 0)
	{
		printf("System: Object array empty, nothing to free\n");
		return;
	}
	if(FreedIndex >= NodePosition || FreedIndex >= PROGRAM_MAX_OBJECTS)
	{
		printf("System: Out of bounds on free list\n");
		return;
	}

	NodeList.Push(FreedIndex);
	Node[FreedIndex].Visible = VIS_STATUS_FREED;
}

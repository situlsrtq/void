#include "scene.h"

void mesh_info_t::AddNode(uint32_t index)
{
	linked_node_t* node = node_list;
	while(node != 0x0)
	{
		node = node->next;
	}

	node = (linked_node_t*)UTIL::Malloc(sizeof(linked_node_t));
	node->index = index;
	node->next = 0x0;
}

void mesh_info_t::RemoveNode(uint32_t index)
{
	linked_node_t* node = node_list;
	if(node == 0x0)
	{
		printf("System: could not remove node - not in mesh list\n");
		return;
	}

	linked_node_t* prev = node_list;
	while(node->index != index)
	{
		if(node == 0x0)
		{
			printf("System: could not remove node - not in mesh list\n");
			return;
		}

		prev = node;
		node = node->next;
	}

	if(node == node_list)
	{
		node_list = node->next;
	}
	else
	{
		prev->next = node->next;
	}

	UTIL::Free(node);
	return;
}

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

void scene_info_t::FreeMesh(uint32_t FreedIndex)
{
	if(MeshPosition == 0)
	{
		printf("System: Object array empty, nothing to free\n");
		return;
	}
	if(FreedIndex >= MeshPosition || FreedIndex >= PROGRAM_MAX_OBJECTS)
	{
		printf("System: Out of bounds on free list\n");
		return;
	}

	while(Mesh[FreedIndex].node_list != 0x0)
	{
		FreeNode(Mesh[FreedIndex].node_list->index);
	}

	PrimList.Push(Mesh[FreedIndex].base_index, Mesh[FreedIndex].size);
	MeshList.Push(FreedIndex);
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

	Mesh[Node->MeshIndex].AddNode(index);

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

	Mesh[Node[FreedIndex].MeshIndex].RemoveNode(FreedIndex);

	NodeList.Push(FreedIndex);
	Node[FreedIndex].Visible = VIS_STATUS_FREED;
}

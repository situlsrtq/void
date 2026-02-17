#include "scene.h"

void mesh_info_t::AddNode(u32 index)
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

void mesh_info_t::RemoveNode(u32 index)
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

u32 scene_info_t::AddMesh(u32 num_primitives)
{
	u32 mesh_id;

	if(MeshList.NextFreePosition > 0)
	{
		mesh_id = MeshList.Pop();
	}
	else
	{
		mesh_id = MeshPosition;

		if(mesh_id >= PROGRAM_MAX_OBJECTS)
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

		Mesh[mesh_id].base_index = PrimPosition;
		Mesh[mesh_id].size = num_primitives;
		PrimPosition += num_primitives;
		return mesh_id;
	}

	Mesh[mesh_id].base_index = PrimList.Pop(num_primitives);
	Mesh[mesh_id].size = num_primitives;
	return mesh_id;
}

void scene_info_t::FreeMesh(u32 mesh_id)
{
	if(MeshPosition == 0)
	{
		printf("System: Object array empty, nothing to free\n");
		return;
	}
	if(mesh_id >= MeshPosition || mesh_id >= PROGRAM_MAX_OBJECTS)
	{
		printf("System: Out of bounds on free list\n");
		return;
	}

	while(Mesh[mesh_id].node_list != 0x0)
	{
		FreeNode(Mesh[mesh_id].node_list->index);
	}

	PrimList.Push(Mesh[mesh_id].base_index, Mesh[mesh_id].size);
	MeshList.Push(mesh_id);
}

void scene_info_t::AddPrimitive(const primitive_create_info_t& CreateInfo, u32 prim_id)
{
	Prim[prim_id] = CreateInfo;
	return;
}

uint32_t scene_info_t::AddNode(const node_create_info_t& CreateInfo, const glm::mat4& ModelIn)
{
	u32 node_id;

	if(NodeList.NextFreePosition > 0)
	{
		node_id = NodeList.Pop();
	}
	else
	{
		node_id = NodePosition;

		if(node_id >= PROGRAM_MAX_OBJECTS)
		{
			printf("System: Object Limit Reached\n");
			return OBJECT_ALLOC_ERROR;
		}

		NodePosition++;
	}

	Node[node_id].MeshIndex = CreateInfo.MeshIndex;
	Node[node_id].node_id = node_id;
	ModelMatrix[node_id] = ModelIn;

	Mesh[Node->MeshIndex].AddNode(node_id);

	return node_id;
}

void scene_info_t::FreeNode(u32 node_id)
{
	if(NodePosition == 0)
	{
		printf("System: Object array empty, nothing to free\n");
		return;
	}
	if(node_id >= NodePosition || node_id >= PROGRAM_MAX_OBJECTS)
	{
		printf("System: Out of bounds on free list\n");
		return;
	}

	Mesh[Node[node_id].MeshIndex].RemoveNode(node_id);

	NodeList.Push(node_id);
}

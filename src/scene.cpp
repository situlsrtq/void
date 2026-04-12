#include "scene.h"

void mesh_info_t::add_node(u32 index)
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

void mesh_info_t::remove_node(u32 index)
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

u32 scene_info_t::add_mesh(u32 num_primitives)
{
	u32 mesh_id;

	mesh_id = mesh_list.pop();

	mesh[mesh_id].base_index = prim_list.pop(num_primitives);
	mesh[mesh_id].size = num_primitives;
	return mesh_id;
}

void scene_info_t::free_mesh(u32 mesh_id)
{
	while(mesh[mesh_id].node_list != 0x0)
	{
		free_node(mesh[mesh_id].node_list->index);
	}

	prim_list.push(mesh[mesh_id].base_index, mesh[mesh_id].size);
	mesh_list.push(mesh_id);
}

void scene_info_t::add_primitive(const primitive_create_info_t& create_info, u32 prim_id)
{
	prim[prim_id] = create_info;
	return;
}

uint32_t scene_info_t::add_node(const node_create_info_t& create_info, const glm::mat4& model_in)
{
	u32 node_id;

	node_id = node_list.pop();

	node[node_id].mesh_index = create_info.mesh_index;
	node[node_id].node_id = node_id;
	model_matrix[node_id] = model_in;

	mesh[node->mesh_index].add_node(node_id);

	return node_id;
}

void scene_info_t::free_node(u32 node_id)
{
	mesh[node[node_id].mesh_index].remove_node(node_id);

	node_list.push(node_id);
}

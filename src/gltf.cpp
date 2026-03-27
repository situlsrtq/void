#include "gltf.h"

void get_node_matrix(glm::mat4* m, cgltf_node* node)
{
	glm::vec3 trv = {node->translation[0], node->translation[1], node->translation[2]};
	glm::vec3 scl = {node->scale[0], node->scale[1], node->scale[2]};
	glm::quat p = {node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]};
	glm::mat4 r = glm::mat4_cast(p);

	*m = glm::mat4(1.0f);
	*m = glm::translate(*m, trv);
	*m *= r;
	*m = glm::scale(*m, scl);
}

int load_indices(vertex_buffer_info_t* vbuffer_state, primitive_create_info_t* create_info, const cgltf_primitive* prim,
		const u8* data_base_addr)
{
	u8 ebo_padding = 0;
	u64 count = prim->indices->count;
	u8 stride = prim->indices->stride;
	u64 rel_buffer_offset = prim->indices->offset;
	u64 abs_buffer_offset = prim->indices->buffer_view->offset;

	GLint indextype = 0;
	if(prim->indices->component_type == cgltf_component_type_r_8u)
	{
		indextype = GL_UNSIGNED_BYTE;
		ebo_padding = 4 - (count % 4);
		ebo_padding = (ebo_padding == 4) ? 0 : ebo_padding;
	}
	if(prim->indices->component_type == cgltf_component_type_r_16u)
	{
		indextype = GL_UNSIGNED_SHORT;
		ebo_padding = (count % 2) * 2;
	}
	if(prim->indices->component_type == cgltf_component_type_r_32u)
	{
		indextype = GL_UNSIGNED_INT;
		ebo_padding = 0;
	}
	if(indextype == 0)
	{
		printf("GLTF Load: No GL-compatible index type: %d", indextype);
		return EXIT_FAILURE;
	}

	glNamedBufferSubData(vbuffer_state->vbuffer_array[INDEX_BUFFER], vbuffer_state->index_byte_offset,
			     count * stride, (void*)(data_base_addr + abs_buffer_offset + rel_buffer_offset));

	create_info->index_info.index_type = indextype;
	create_info->index_info.index_count = count;
	create_info->index_info.ebo_byte_offset = vbuffer_state->index_byte_offset;

	// Since this is a shared buffer for the entire scene, pad all writes to nearest 32bit
	// boundary to prevent misalignment when indextype changes from a smaller type to a
	// larger one
	vbuffer_state->index_byte_offset += (count * stride) + ebo_padding;

	return EXIT_SUCCESS;
}

cgltf_attribute* find_attr_type(const cgltf_primitive* prim, cgltf_attribute_type type)
{
	cgltf_attribute* attr = &prim->attributes[0];

	for(u32 i = 0; i < prim->attributes_count; i++)
	{
		if(prim->attributes[i].type != type)
		{
			continue;
		}

		attr = &prim->attributes[i];
		return attr;
	}

	attr = 0x0;
	return attr;
}

int load_vertices(vertex_buffer_info_t* vbuffer_state, primitive_create_info_t* create_info, const cgltf_primitive* prim,
		 const u8* data_base_addr)
{
	cgltf_attribute* attr = find_attr_type(prim, cgltf_attribute_type_position);
	if(!attr)
	{
		printf("GLTF: unsupported vertex format (no position attr)\n");
		return EXIT_FAILURE;
	}
	create_info->vertex_info.vattr_count = attr->data->count;
	u64 relbufferoffset = attr->data->offset;
	u64 absbufferoffset = attr->data->buffer_view->offset;

	u8 stride = attr->data->stride;
	if(stride != 12)
	{
		printf("GLTF: unsupported vertex layout (position)\n");
		return EXIT_FAILURE;
	}

	glNamedBufferSubData(vbuffer_state->vbuffer_array[POS_BUFFER], vbuffer_state->pos_offset * stride,
			     create_info->vertex_info.vattr_count * stride,
			     (void*)(data_base_addr + absbufferoffset + relbufferoffset));

	create_info->vertex_info.vertex_offset = vbuffer_state->pos_offset;

	// No need to pad this offset, because we're tracking the count, not an address
	vbuffer_state->pos_offset += create_info->vertex_info.vattr_count;

	attr = find_attr_type(prim, cgltf_attribute_type_normal);
	if(!attr)
	{
		printf("GLTF: unsupported vertex format (no normal attr)\n");
		return EXIT_FAILURE;
	}
	relbufferoffset = attr->data->offset;
	absbufferoffset = attr->data->buffer_view->offset;

	stride = attr->data->stride;
	if(stride != 12)
	{
		printf("GLTF: unsupported vertex layout (normal)\n");
		return EXIT_FAILURE;
	}

	glNamedBufferSubData(vbuffer_state->vbuffer_array[NORM_BUFFER], vbuffer_state->norm_offset * stride,
			     create_info->vertex_info.vattr_count * stride,
			     (void*)(data_base_addr + absbufferoffset + relbufferoffset));

	vbuffer_state->norm_offset += create_info->vertex_info.vattr_count;

	// Only requre this attribute if model uses a normal map
	attr = find_attr_type(prim, cgltf_attribute_type_tangent);
	if(prim->material->normal_texture.texture && !attr)
	{
		printf("GLTF: unsupported vertex format (no tangent attr on normal mapped mesh)\n");
		return EXIT_FAILURE;
	}

	if(attr)
	{
		relbufferoffset = attr->data->offset;
		absbufferoffset = attr->data->buffer_view->offset;

		stride = attr->data->stride;
		if(stride != 16)
		{
			printf("GLTF: unsupported vertex layout (tangent)\n");
			return EXIT_FAILURE;
		}

		glNamedBufferSubData(vbuffer_state->vbuffer_array[TAN_BUFFER], vbuffer_state->tan_offset * stride,
				     create_info->vertex_info.vattr_count * stride,
				     (void*)(data_base_addr + absbufferoffset + relbufferoffset));

		vbuffer_state->tan_offset += create_info->vertex_info.vattr_count;
	}

	attr = find_attr_type(prim, cgltf_attribute_type_texcoord);
	if(!attr)
	{
		printf("GLTF: unsupported vertex format (no texcoord attr)\n");
		return EXIT_FAILURE;
	}
	relbufferoffset = attr->data->offset;
	absbufferoffset = attr->data->buffer_view->offset;

	stride = attr->data->stride;
	if(stride != 8)
	{
		printf("GLTF: unsupported vertex layout (texcoord)\n");
		return EXIT_FAILURE;
	}

	glNamedBufferSubData(vbuffer_state->vbuffer_array[TEX_BUFFER], vbuffer_state->tex_offset * stride,
			     create_info->vertex_info.vattr_count * stride,
			     (void*)(data_base_addr + absbufferoffset + relbufferoffset));

	vbuffer_state->tex_offset += create_info->vertex_info.vattr_count;

	return EXIT_SUCCESS;
}

int upload_texture_2d(unsigned int texture, const u8* data_base_addr, u64 offset, u32 size)
{
	int width;
	int height;
	u32 format;
	u32 internalformat;
	int nchannels = 0;
	unsigned char* texdata = 0x0;

	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	texdata = stbi_load_from_memory(data_base_addr + offset, size, &width, &height, &nchannels, 0);

	switch(nchannels)
	{
		case 2:
			format = GL_RG;
			internalformat = GL_RG8;
			break;
		case 3:
			format = GL_RGB;
			internalformat = GL_RGB8;
			break;
		case 4:
			format = GL_RGBA;
			internalformat = GL_RGBA8;
			break;
		default:
			printf("GLTF: Could not obtain valid texture format from image\n");
			return EXIT_FAILURE;
	}

	glTextureStorage2D(texture, 1, internalformat, width, height);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, texdata);

	stbi_image_free(texdata);

	return EXIT_SUCCESS;
}

int texture_to_gpu(hash_table_t* hash_table, primitive_create_info_t* create_info, const u32 texture, const cgltf_texture* filetex,
		 const u8* data_base_addr)
{
	int res;
	u32 hash_res;
	u32 texsize;
	u64 absbufferoffset;

	hash_res = rh_hash_find(hash_table, filetex->image->name, strlen(filetex->image->name)+1);
	if(hash_res == KEY_NOT_FOUND)
	{
		texsize = filetex->image->buffer_view->size;
		absbufferoffset = filetex->image->buffer_view->offset;

		res = upload_texture_2d(create_info->texture_info.tex_array[texture], data_base_addr, absbufferoffset, texsize);
		if(res == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		rh_hash_insert(hash_table, filetex->image->name, strlen(filetex->image->name)+1,
				     create_info->texture_info.tex_array[texture]);
	}
	else
	{
		create_info->texture_info.tex_array[texture] = hash_res;
	}

	return EXIT_SUCCESS;
}

int load_textures(hash_table_t* hash_table, u32 tex_count, primitive_create_info_t* create_info, const cgltf_primitive* prim,
		 const u8* data_base_addr)
{
	int res;

	glCreateTextures(GL_TEXTURE_2D, tex_count, create_info->texture_info.tex_array);

	cgltf_texture* filetex = prim->material->pbr_metallic_roughness.base_color_texture.texture;
	if(filetex)
	{
		res = texture_to_gpu(hash_table, create_info, 0, filetex, data_base_addr);
		if(res == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	filetex = prim->material->pbr_metallic_roughness.metallic_roughness_texture.texture;
	if(filetex)
	{
		res = texture_to_gpu(hash_table, create_info, 1, filetex, data_base_addr);
		if(res == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	filetex = prim->material->normal_texture.texture;
	if(filetex)
	{
		res = texture_to_gpu(hash_table, create_info, 2, filetex, data_base_addr);
		if(res == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int load_scene_from_glb(const char* scene_file, window_handler_t*& win_hnd, unsigned int* vao,
		     vertex_buffer_info_t* vbuffer_state, unsigned int tex_count)
{
	printf("Validating glb file: %s\n", scene_file);

	int res;
	size_t num_scene_bytes;
	res = UTIL::GetFileSize(scene_file, &num_scene_bytes);
	if(res == EXIT_FAILURE)
	{
		printf("Resources: could not access scene file %s\n", scene_file);
		return EXIT_FAILURE;
	}

	cgltf_options opt;
	memset(&opt, 0, sizeof(opt));
	cgltf_data* data = 0x0;

	cgltf_result cgl_res = cgltf_parse_file(&opt, scene_file, &data);
	if(cgl_res != cgltf_result_success)
	{
		printf("GLTF Load: could not parse file %s", scene_file);
		return EXIT_FAILURE;
	}

	cgl_res = cgltf_load_buffers(&opt, data, scene_file);
	if(cgl_res != cgltf_result_success)
	{
		printf("GLTF Load: could not validate file %s", scene_file);
		return EXIT_FAILURE;
	}

	cgl_res = cgltf_validate(data);
	if(cgl_res != cgltf_result_success)
	{
		printf("GLTF Load: could not validate file %s", scene_file);
		return EXIT_FAILURE;
	}

	u8* data_base_addr = (u8*)data->buffers->data;

	glBindVertexArray(*vao);

	printf("Loading gltf data: %s\n", scene_file);

	primitive_create_info_t prim_info;
	node_create_info_t node_info;

	u32 mesh_index = 0;
	for(u32 i = 0; i < data->nodes_count; i++)
	{
		memset(&node_info, 0, sizeof(node_info));
		cgltf_node* node = &data->nodes[i];

		glm::mat4 node_matrix;
		get_node_matrix(&node_matrix, node);
		glm::mat3 node_inv_trans = glm::mat3(inverse(transpose(node_matrix)));

		cgltf_mesh* mesh = node->mesh;
		mesh_index = rh_hash_find(&win_hnd->hash_table, mesh->name, strlen(mesh->name)+1);
		if(mesh_index == KEY_NOT_FOUND)
		{
			mesh_index = win_hnd->scene.add_mesh(mesh->primitives_count);
			if(mesh_index == OBJECT_ALLOC_ERROR)
			{
				return EXIT_FAILURE;
			}

			for(u32 t = 0; t < mesh->primitives_count; t++)
			{
				memset(&prim_info, 0, sizeof(prim_info));
				prim_info.color = {1.0f, 1.0f, 1.0f};
				prim_info.model_inv_trans = node_inv_trans;

				cgltf_primitive* prim = &mesh->primitives[t];

				if(prim->indices)
				{
					res = load_indices(vbuffer_state, &prim_info, prim, data_base_addr);
					if(res == EXIT_FAILURE)
					{
						return EXIT_FAILURE;
					}
				}

				res = load_vertices(vbuffer_state, &prim_info, prim, data_base_addr);
				if(res == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}

				res = load_textures(&win_hnd->hash_table, tex_count, &prim_info, prim, data_base_addr);
				if(res == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}

				win_hnd->scene.add_primitive(prim_info, win_hnd->scene.mesh[mesh_index].base_index + t);
			}

			node_info.mesh_index = mesh_index;
			win_hnd->scene.add_node(node_info, node_matrix);

			rh_hash_insert(&win_hnd->hash_table, mesh->name, strlen(mesh->name)+1, mesh_index);
		}

		node_info.mesh_index = mesh_index;
		win_hnd->scene.add_node(node_info, node_matrix);
	}

	cgltf_free(data);

	return EXIT_SUCCESS;
}

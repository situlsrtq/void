#include "gltf.h"

void GetNodeMatrix(glm::mat4* m, cgltf_node* node)
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

int LoadIndices(vertex_buffer_info_t* VBufferState, primitive_create_info_t* CreateInfo, const cgltf_primitive* prim,
		const uint8_t* DataBaseAddr)
{
	uint8_t EBOPadding = 0;
	uint64_t count = prim->indices->count;
	uint8_t stride = prim->indices->stride;
	uint64_t relbufferoffset = prim->indices->offset;
	uint64_t absbufferoffset = prim->indices->buffer_view->offset;

	GLint indextype = 0;
	if(prim->indices->component_type == cgltf_component_type_r_8u)
	{
		indextype = GL_UNSIGNED_BYTE;
		EBOPadding = 4 - (count % 4);
		EBOPadding = (EBOPadding == 4) ? 0 : EBOPadding;
	}
	if(prim->indices->component_type == cgltf_component_type_r_16u)
	{
		indextype = GL_UNSIGNED_SHORT;
		EBOPadding = (count % 2) * 2;
	}
	if(prim->indices->component_type == cgltf_component_type_r_32u)
	{
		indextype = GL_UNSIGNED_INT;
		EBOPadding = 0;
	}
	if(indextype == 0)
	{
		printf("GLTF Load: No GL-compatible index type: %d", indextype);
		return EXIT_FAILURE;
	}

	glNamedBufferSubData(VBufferState->VBufferArray[INDEX_BUFFER], VBufferState->CurrIndexByteOffset,
			     count * stride, (void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

	CreateInfo->IndexInfo.IndexType = indextype;
	CreateInfo->IndexInfo.IndexCount = count;
	CreateInfo->IndexInfo.ByteOffsetEBO = VBufferState->CurrIndexByteOffset;

	// Since this is a shared buffer for the entire scene, pad all writes to nearest 32bit
	// boundary to prevent misalignment when indextype changes from a smaller type to a
	// larger one
	VBufferState->CurrIndexByteOffset += (count * stride) + EBOPadding;

	return EXIT_SUCCESS;
}

cgltf_attribute* FindAttrType(const cgltf_primitive* prim, cgltf_attribute_type type)
{
	cgltf_attribute* attr = &prim->attributes[0];

	for(uint32_t i = 0; i < prim->attributes_count; i++)
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

int LoadVertices(vertex_buffer_info_t* VBufferState, primitive_create_info_t* CreateInfo, const cgltf_primitive* prim,
		 const uint8_t* DataBaseAddr)
{
	cgltf_attribute* attr = FindAttrType(prim, cgltf_attribute_type_position);
	if(!attr)
	{
		printf("GLTF: unsupported vertex format (no position attr)\n");
		return EXIT_FAILURE;
	}
	CreateInfo->VertexInfo.VAttrCount = attr->data->count;
	uint64_t relbufferoffset = attr->data->offset;
	uint64_t absbufferoffset = attr->data->buffer_view->offset;

	uint8_t stride = attr->data->stride;
	if(stride != 12)
	{
		printf("GLTF: unsupported vertex layout (position)\n");
		return EXIT_FAILURE;
	}

	glNamedBufferSubData(VBufferState->VBufferArray[POS_BUFFER], VBufferState->CurrPosOffset * stride,
			     CreateInfo->VertexInfo.VAttrCount * stride,
			     (void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

	CreateInfo->VertexInfo.VertexOffset = VBufferState->CurrPosOffset;

	// No need to pad this offset, because we're tracking the count, not an address
	VBufferState->CurrPosOffset += CreateInfo->VertexInfo.VAttrCount;

	attr = FindAttrType(prim, cgltf_attribute_type_normal);
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

	glNamedBufferSubData(VBufferState->VBufferArray[NORM_BUFFER], VBufferState->CurrNormOffset * stride,
			     CreateInfo->VertexInfo.VAttrCount * stride,
			     (void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

	VBufferState->CurrNormOffset += CreateInfo->VertexInfo.VAttrCount;

	// Only requre this attribute if model uses a normal map
	attr = FindAttrType(prim, cgltf_attribute_type_tangent);
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

		glNamedBufferSubData(VBufferState->VBufferArray[TAN_BUFFER], VBufferState->CurrTanOffset * stride,
				     CreateInfo->VertexInfo.VAttrCount * stride,
				     (void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

		VBufferState->CurrTanOffset += CreateInfo->VertexInfo.VAttrCount;
	}

	attr = FindAttrType(prim, cgltf_attribute_type_texcoord);
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

	glNamedBufferSubData(VBufferState->VBufferArray[TEX_BUFFER], VBufferState->CurrTexOffset * stride,
			     CreateInfo->VertexInfo.VAttrCount * stride,
			     (void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

	VBufferState->CurrTexOffset += CreateInfo->VertexInfo.VAttrCount;

	return EXIT_SUCCESS;
}

int UploadTexture_2D(unsigned int Texture, const uint8_t* DataBaseAddr, uint64_t Offset, uint32_t Size)
{
	int width;
	int height;
	uint32_t format;
	uint32_t internalformat;
	int nchannels = 0;
	unsigned char* texdata = 0x0;

	glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	texdata = stbi_load_from_memory(DataBaseAddr + Offset, Size, &width, &height, &nchannels, 0);

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

	glTextureStorage2D(Texture, 1, internalformat, width, height);
	glTextureSubImage2D(Texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, texdata);

	stbi_image_free(texdata);

	return EXIT_SUCCESS;
}

int TextureToGPU(primitive_create_info_t* CreateInfo, const uint32_t Texture, const cgltf_texture* filetex,
		 const uint8_t* DataBaseAddr)
{
	int res;
	uint32_t hash_res;
	uint32_t texsize;
	uint64_t absbufferoffset;

	hash_res = g_test_table->Find(filetex->image->name, strlen(filetex->image->name)+1);
	if(hash_res == KEY_NOT_FOUND)
	{
		texsize = filetex->image->buffer_view->size;
		absbufferoffset = filetex->image->buffer_view->offset;

		res = UploadTexture_2D(CreateInfo->TexInfo.TexArray[Texture], DataBaseAddr, absbufferoffset, texsize);
		if(res == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		g_test_table->Insert(filetex->image->name, strlen(filetex->image->name)+1,
				     CreateInfo->TexInfo.TexArray[Texture]);
	}
	else
	{
		CreateInfo->TexInfo.TexArray[Texture] = hash_res;
	}

	return EXIT_SUCCESS;
}

int LoadTextures(uint32_t TexCount, primitive_create_info_t* CreateInfo, const cgltf_primitive* prim,
		 const uint8_t* DataBaseAddr)
{
	int res;

	glCreateTextures(GL_TEXTURE_2D, TexCount, CreateInfo->TexInfo.TexArray);

	cgltf_texture* filetex = prim->material->pbr_metallic_roughness.base_color_texture.texture;
	if(filetex)
	{
		res = TextureToGPU(CreateInfo, 0, filetex, DataBaseAddr);
		if(res == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	filetex = prim->material->pbr_metallic_roughness.metallic_roughness_texture.texture;
	if(filetex)
	{
		res = TextureToGPU(CreateInfo, 1, filetex, DataBaseAddr);
		if(res == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	filetex = prim->material->normal_texture.texture;
	if(filetex)
	{
		res = TextureToGPU(CreateInfo, 2, filetex, DataBaseAddr);
		if(res == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int LoadSceneFromGLB(const char* SceneFile, window_handler_t*& WinHND, unsigned int* VAO,
		     vertex_buffer_info_t* VBufferState, unsigned int TexCount)
{
	printf("Validating glb file: %s\n", SceneFile);

	int res;
	size_t NumSceneBytes;
	res = UTIL::GetFileSize(SceneFile, &NumSceneBytes);
	if(res == EXIT_FAILURE)
	{
		printf("Resources: could not access scene file %s\n", SceneFile);
		return EXIT_FAILURE;
	}

	cgltf_options opt;
	memset(&opt, 0, sizeof(opt));
	cgltf_data* data = 0x0;

	cgltf_result cgl_res = cgltf_parse_file(&opt, SceneFile, &data);
	if(cgl_res != cgltf_result_success)
	{
		printf("GLTF Load: could not parse file %s", SceneFile);
		return EXIT_FAILURE;
	}

	cgl_res = cgltf_load_buffers(&opt, data, SceneFile);
	if(cgl_res != cgltf_result_success)
	{
		printf("GLTF Load: could not validate file %s", SceneFile);
		return EXIT_FAILURE;
	}

	cgl_res = cgltf_validate(data);
	if(cgl_res != cgltf_result_success)
	{
		printf("GLTF Load: could not validate file %s", SceneFile);
		return EXIT_FAILURE;
	}

	uint8_t* DataBaseAddr = (uint8_t*)data->buffers->data;

	glBindVertexArray(*VAO);

	printf("Loading gltf data: %s\n", SceneFile);

	primitive_create_info_t PrimInfo;
	node_create_info_t NodeInfo;

	uint32_t mesh_index = 0;
	for(uint32_t i = 0; i < data->nodes_count; i++)
	{
		memset(&NodeInfo, 0, sizeof(NodeInfo));
		cgltf_node* node = &data->nodes[i];

		glm::mat4 node_matrix;
		GetNodeMatrix(&node_matrix, node);
		glm::mat3 node_inv_trans = glm::mat3(inverse(transpose(node_matrix)));

		cgltf_mesh* mesh = node->mesh;
		mesh_index = g_test_table->Find(mesh->name, strlen(mesh->name)+1);
		if(mesh_index == KEY_NOT_FOUND)
		{
			mesh_index = WinHND->Scene.AddMesh(mesh->primitives_count);
			if(mesh_index == OBJECT_ALLOC_ERROR)
			{
				return EXIT_FAILURE;
			}

			for(uint32_t t = 0; t < mesh->primitives_count; t++)
			{
				memset(&PrimInfo, 0, sizeof(PrimInfo));
				PrimInfo.Color = {1.0f, 1.0f, 1.0f};
				PrimInfo.ModelInvTrans = node_inv_trans;

				cgltf_primitive* prim = &mesh->primitives[t];

				if(prim->indices)
				{
					res = LoadIndices(VBufferState, &PrimInfo, prim, DataBaseAddr);
					if(res == EXIT_FAILURE)
					{
						return EXIT_FAILURE;
					}
				}

				res = LoadVertices(VBufferState, &PrimInfo, prim, DataBaseAddr);
				if(res == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}

				res = LoadTextures(TexCount, &PrimInfo, prim, DataBaseAddr);
				if(res == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}

				WinHND->Scene.AddPrimitive(PrimInfo, WinHND->Scene.Mesh[mesh_index].base_index + t);
			}

			NodeInfo.Visible = VIS_STATUS_VISIBLE;
			NodeInfo.MeshIndex = mesh_index;
			WinHND->Scene.AddNode(NodeInfo, node_matrix);

			g_test_table->Insert(mesh->name, strlen(mesh->name)+1, mesh_index);
		}

		NodeInfo.Visible = VIS_STATUS_VISIBLE;
		NodeInfo.MeshIndex = mesh_index;
		WinHND->Scene.AddNode(NodeInfo, node_matrix);
	}

	cgltf_free(data);

	return EXIT_SUCCESS;
}

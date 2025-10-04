#include "scene.h"

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
	printf("GLTF: Attribute type not found %s\n", "peepee, map to attr_type later");
	return attr;
}

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

int UploadTextureGPU_2Dmipped(unsigned int Texture, const uint8_t* DataBaseAddr, uint64_t Offset, uint32_t Size)
{
	int width;
	int height;
	uint32_t format;
	int nchannels = 0;
	unsigned char* texdata = 0x0;

	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	texdata = stbi_load_from_memory(DataBaseAddr + Offset, Size, &width, &height, &nchannels, 0);

	switch(nchannels)
	{
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			printf("Scene: Could not obtain valid texture format from image\n");
			return EXIT_FAILURE;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texdata);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(texdata);

	return EXIT_SUCCESS;
}

int LoadSceneFromGLB(const char* SceneFile, window_handler_t*& WinHND, unsigned int* VAO, unsigned int* EBO,
		     unsigned int* VBO, unsigned int VAStride, unsigned int VACount, unsigned int TexCount)
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

	glBindVertexArray(*VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumSceneBytes, 0x0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, NumSceneBytes, 0x0, GL_DYNAMIC_DRAW);

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
	uint64_t OffsetEBO = 0;
	uint64_t OffsetVBO = 0;
	uint8_t EBOPadding = 0;
	geometry_create_info_t CreateInfo;

	printf("Loading scene data: %s\n", SceneFile);
	printf("If your textures are PNGs or other non-delivery formats, this will take a long fucking time\n");

	for(uint32_t i = 0; i < data->nodes_count; i++)
	{
		cgltf_node* node = &data->nodes[i];

		uint64_t absbufferoffset;
		uint64_t relbufferoffset;
		uint64_t count;
		uint8_t stride;
		glm::mat4 nodematrix;
		GetNodeMatrix(&nodematrix, node);

		cgltf_mesh* mesh = node->mesh;
		for(uint32_t t = 0; t < mesh->primitives_count; t++)
		{
			memset(&CreateInfo, 0, sizeof(CreateInfo));
			CreateInfo.Color = {1.0f, 1.0f, 1.0f};
			CreateInfo.Model = nodematrix;

			cgltf_primitive* prim = &mesh->primitives[t];
			cgltf_attribute* attr;

			if(prim->attributes_count != VACount)
			{
				printf("GLTF Load: unsupported vertex format: %s\n", SceneFile);
				return EXIT_FAILURE;
			}

			if(prim->indices)
			{
				count = prim->indices->count;
				stride = prim->indices->stride;
				relbufferoffset = prim->indices->offset;
				absbufferoffset = prim->indices->buffer_view->offset;
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
					printf("GLTF Load: No GL-compatible index type: %s", SceneFile);
					return EXIT_FAILURE;
				}

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, OffsetEBO, count * stride,
						(void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

				CreateInfo.IndexType = indextype;
				CreateInfo.IndexCount = count;
				CreateInfo.ByteOffsetEBO = OffsetEBO;

				// Since this is a shared buffer for the entire scene, pad all writes to nearest 32bit
				// boundary to prevent misalignment when indextype changes from a smaller type to a
				// larger one
				OffsetEBO += (count * stride) + EBOPadding;
			}

			attr = &prim->attributes[0];
			count = attr->data->count;
			relbufferoffset = attr->data->offset;
			absbufferoffset = attr->data->buffer_view->offset;

			stride = attr->data->stride;
			if((stride != VAStride) || (strcmp(attr->name, "POSITION") != 0)
			   || (strcmp(prim->attributes[1].name, "NORMAL") != 0)
			   || (strcmp(prim->attributes[2].name, "TEXCOORD_0") != 0))
			{
				printf("GLTF Load: unsupported vertex layout: %s\n", SceneFile);
				return EXIT_FAILURE;
			}

			glBindBuffer(GL_ARRAY_BUFFER, *VBO);
			glBufferSubData(GL_ARRAY_BUFFER, OffsetVBO * stride, count * stride,
					(void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

			CreateInfo.VAttrCount = count;
			CreateInfo.OffsetVBO = OffsetVBO;

			// No need to pad this offset, because we're not mixing vertex formats within a single buffer
			OffsetVBO += count;

			uint32_t texsize;
			glGenTextures(TexCount, CreateInfo.TexInfo.TexArray);

			cgltf_texture* filetex = prim->material->pbr_metallic_roughness.base_color_texture.texture;
			if(filetex)
			{
				texsize = filetex->image->buffer_view->size;
				absbufferoffset = filetex->image->buffer_view->offset;
				UploadTextureGPU_2Dmipped(CreateInfo.TexInfo.TexArray[0], DataBaseAddr, absbufferoffset,
							  texsize);
			}

			filetex = prim->material->pbr_metallic_roughness.metallic_roughness_texture.texture;
			if(filetex)
			{
				texsize = filetex->image->buffer_view->size;
				absbufferoffset = filetex->image->buffer_view->offset;
				UploadTextureGPU_2Dmipped(CreateInfo.TexInfo.TexArray[1], DataBaseAddr, absbufferoffset,
							  texsize);
			}

			filetex = prim->material->normal_texture.texture;
			if(filetex)
			{
				texsize = filetex->image->buffer_view->size;
				absbufferoffset = filetex->image->buffer_view->offset;
				UploadTextureGPU_2Dmipped(CreateInfo.TexInfo.TexArray[2], DataBaseAddr, absbufferoffset,
							  texsize);
			}

			WinHND->GeometryObjects.Alloc(CreateInfo);
		}
	}

	cgltf_free(data);

	return EXIT_SUCCESS;
}

#ifndef VOID_GLTF_LOADER_H
#define VOID_GLTF_LOADER_H

#include <stdio.h>

#include <cgltf/cgltf.h>
#include <stb/stb_image.h>
#include <glad/glad.h>

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "window.h"
#include "rendertypes.h"
#include "scene.h"
#include "u_hash.h"

int LoadSceneFromGLB(const char* SceneFile, window_handler_t*& WinHND, unsigned int* VAO,
		     vertex_buffer_info_t* VBufferState, unsigned int TexCount);

#endif

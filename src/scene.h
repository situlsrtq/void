#ifndef VOID_SCENE_H
#define VOID_SCENE_H

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
#include "u_mem.h"

int LoadSceneFromGLB(const char* SceneFile, window_handler_t*& WinHND, unsigned int* VAO, unsigned int* EBO,
		     unsigned int* VBO, unsigned int VAStride, unsigned int VACount, unsigned int TexCount);

#endif

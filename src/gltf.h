#ifndef VOID_GLTF_LOADER_H
#define VOID_GLTF_LOADER_H

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
#include "grid.h"
#include "u_hash.h"
#include "u_types.h"

int load_scene_from_glb(const char* scene_file, window_handler_t*& win_hnd, unsigned int* vao,
		     vertex_buffer_info_t* vbuffer_state, unsigned int tex_count);

#endif

#ifndef VOID_RENDERTHREAD_H
#define VOID_RENDERTHREAD_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "tracy_wrapper.h"
#ifdef TRACY_ENABLE
	#include <tracy/TracyOpenGL.hpp>
	#define GPU_ZONE(name) TracyGpuZone(name)
#else
	#define GPU_ZONE(name)
#endif

#include "PAL.h"

#include "window.h"
#include "u_mem.h"
#include "u_types.h"
#include "scene.h"
#include "gltf.h"

#define VOID_HFOV_DEFAULT 45.0f
#define KEY_PRESS GLFW_PRESS
#define KEY_RELEASE GLFW_RELEASE

// TODO: Handle in frame loop, not with a callback maybe?

void frame_resize_callback(GLFWwindow* window, int width, int height);

// TODO: Split out into Input Handling module

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_pos_callback(GLFWwindow* window, double mx, double my);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void process_input(window_handler_t* win_hnd, GLFWwindow* window);

// TODO: Split out into UI module

void gen_interface_elements(window_handler_t* win_hnd, bool* help_window, bool* post_window, bool* demo_window);

#endif

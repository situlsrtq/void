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

#include "PAL.h"

#include "window.h"
#include "u_mem.h"
#include "u_types.h"
#include "scene.h"
#include "gltf.h"

#define VOID_HFOV_DEFAULT 45.0f

// int RenderSystemsInit(window_handler_t*& WinHND);

// TODO: Split out into Core/PAL module

void FrameResizeCallback(GLFWwindow* Window, int width, int height);

// TODO: Split out into Input Handling module

void MousePosCallback(GLFWwindow* Window, double mx, double my);
void ProcessInput(GLFWwindow* Window);

// TODO: Split out into UI module

void GenerateInterfaceElements(window_handler_t* WinHND, bool* HelpWindow, bool* PostWindow, bool* DemoWindow);

#endif

#ifndef VOID_RENDERTHREAD_H
#define VOID_RENDERTHREAD_H


#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#endif
#include <cgltf.h>


#include "PAL.h"

#include "window.h"
#include "u_math.h"
#include "u_mem.h"


#define VOID_HFOV_DEFAULT 45.0f

//TODO: put in glb module
#define VOID_VATTR_STRIDE 32
#define VOID_VATTR_COUNT 3


int RenderSystemsInit(window_handler_t*& WinHND);

// TODO: Split out into Core/PAL module

void FrameResizeCallback(GLFWwindow* Window, int width, int height);

// TODO: Split out into Input Handling module

void MousePosCallback(GLFWwindow* Window, double mx, double my);
void ProcessInput(GLFWwindow* Window);

// TODO: Split out into UI module

void GenerateInterfaceElements(window_handler_t *WinHND, bool *HelpWindow, bool *PostWindow, bool *DemoWindow);


#endif

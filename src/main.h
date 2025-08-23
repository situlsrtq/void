#ifndef VOID_RENDERTHREAD_H
#define VOID_RENDERTHREAD_H


#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../vendor/glad/glad.h"
#include "../vendor/GLFW/glfw3.h"

#include "../pal/PAL.h"

#include "shader.h"
#include "picking.h"
#include "camera.h"
#include "window.h"
#include "util/u_math.h"
#include "util/u_mem.h"


// TODO: Split out into Core/PAL module


void FrameResizeCallback(GLFWwindow* Window, int width, int height);


// TODO: Split out into Input Handling module

void MousePosCallback(GLFWwindow* Window, double mx, double my);
void ProcessInput(GLFWwindow* Window);


// TODO: Split out into UI module

void GenerateInterfaceElements(window_handler_t* WinHND, bool* HelpWindow, bool* DemoWindow);


#endif

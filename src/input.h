#ifndef INPUT_H
#define INPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "window.h"

void KeyInputCallback(GLFWwindow* Window, int key, int scancode, int action, int mods);

void MousePosCallback(GLFWwindow* Window, double mx, double my);

#endif

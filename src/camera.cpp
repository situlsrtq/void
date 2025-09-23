#include "camera.h"


// Uses Euler rotation, capped to prevent gimbal lock. In testing, this proved to be
// far more in line with what users naturally expected than free-form matrix rotation
void mbox_camera_t::LookAtMouse(double XOffset, double YOffset)
{
	XOffset *= Sensitivity;
	YOffset *= Sensitivity;

	Yaw += XOffset;
	Pitch += YOffset;

	if (Pitch > 89.0f)
	{
		Pitch = 89.0f;
	}
	if (Pitch < -89.0f)
	{
		Pitch = -89.0f;
	}

	glm::vec3 direction = {};
	direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	direction.y = sin(glm::radians(Pitch));
	direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch ));

	Eye = glm::normalize(direction);
}


// Wrapper for Camera-specific input handling
void mbox_camera_t::Move(GLFWwindow *Window)
{
	if(glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += Eye * Speed;
	}
	if(glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position -= Eye * Speed;
	}
	if(glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position -= RelativeXAxis * Speed;
	}
	if(glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += RelativeXAxis * Speed;
	}
	if(glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += RelativeYAxis * Speed;
	}
	if(glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		Position -= RelativeYAxis * Speed;
	}
}

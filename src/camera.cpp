#include "camera.h"

// Uses Euler rotation, capped to prevent gimbal lock. In testing, this proved to be
// far more in line with what users naturally expected than free-form quat rotation
void camera_info_t::look_at_mouse(double xoffset, double yoffset)
{
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if(pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if(pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	glm::vec3 direction = {};
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	eye = glm::normalize(direction);
}

// Wrapper for Camera-specific input handling
void camera_info_t::move(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += eye * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position -= eye * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position -= rel_x_axis * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += rel_x_axis * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += rel_y_axis * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		position -= rel_y_axis * speed;
	}
}

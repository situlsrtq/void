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

	uMATH::vec3f_t direction = {};
	direction.x = cos((Yaw * RADIAN)) * cos((Pitch * RADIAN));
	direction.y = sin((Pitch * RADIAN));
	direction.z = sin((Yaw * RADIAN)) * cos((Pitch * RADIAN));

	Eye = uMATH::Normalize(direction);
}


// Wrapper for Camera-specific input handling
void mbox_camera_t::Move(GLFWwindow *Window)
{
	if(glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += Scalar(Eye, Speed);
	}
	if(glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position -= Scalar(Eye, Speed);
	}
	if(glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position -= Scalar(RelativeXAxis, Speed);
	}
	if(glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += Scalar(RelativeXAxis, Speed);
	}
	if(glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += Scalar(RelativeYAxis, Speed);
	}
	if(glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		Position -= Scalar(RelativeYAxis, Speed);
	}
}

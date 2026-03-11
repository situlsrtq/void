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
void mbox_camera_t::Move(float dTime)
{
	Speed = 2.5f * dTime;
	RelativeXAxis = glm::normalize(glm::cross(Eye, UpAxis));
	RelativeYAxis = glm::normalize(glm::cross(RelativeXAxis, Eye));

	if (instructions.forward) {
		Position += Eye * Speed;
	}
	if (instructions.backward) {
		Position -= Eye * Speed;
	}
	if (instructions.left) {
		Position -= RelativeXAxis * Speed;
	}
	if (instructions.right) {
		Position += RelativeXAxis * Speed;
	}
	if (instructions.up) {
		Position += RelativeYAxis * Speed;
	}
	if (instructions.down) {
		Position -= RelativeYAxis * Speed;
	}
}

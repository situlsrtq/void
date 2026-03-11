#ifndef MBOX_CAMERA_H
#define MBOX_CAMERA_H


#include <GLFW/glfw3.h>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct camera_move {
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
};

struct mbox_camera_t
{
	// Fixed values - changed by system code
	float Sensitivity = 0.1f;
	float Speed = 0.0f;
	float Yaw = -90.0f;
	float Pitch = 0.0f;

	glm::vec3 Eye = {0.0f,0.0f,-1.0f};
	glm::vec3 UpAxis = {0.0f,1.0f,0.0f};

	// Runtime values - changed by user actions
	glm::vec3 Position = {0.0f, 0.0f, 3.0f};
	glm::vec3 RelativeXAxis = {};
	glm::vec3 RelativeYAxis = {};

	camera_move instructions;

	void LookAtMouse(double XOffset, double YOffset);
	void Move(float dTime);
};


#endif

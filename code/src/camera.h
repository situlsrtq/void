#ifndef MBOX_CAMERA_H
#define MBOX_CAMERA_H

#include <GLFW/glfw3.h>
#include "../inc/util/u_math.h"

struct mbox_camera_t
{
	float Sensitivity = 0.1f;
	float Speed = 0.0f;
	float Yaw = -90.0f;
	float Pitch = 0.0f;

	uMATH::vec3f_t Position = {0.0f, 0.0f, 3.0f};
	uMATH::vec3f_t Eye = {0.0f,0.0f,-1.0f};
	uMATH::vec3f_t UpAxis = {0.0f,1.0f,0.0f};
	uMATH::vec3f_t RelativeXAxis = {};
	uMATH::vec3f_t RelativeYAxis = {};

	void LookAtMouse(double XOffset, double YOffset);
	void Move(GLFWwindow *Window);
};

#endif

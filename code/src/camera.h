#ifndef MBOX_CAMERA_H
#define MBOX_CAMERA_H

#include <GLFW/glfw3.h>
#include "../include/util/u_math.h"

struct mbox_camera_t
{
	float Sensitivity;
	float Speed;
	float Yaw;
	float Pitch;

	uMATH::vec3f_t Position = {};
	uMATH::vec3f_t Eye = {};
	uMATH::vec3f_t UpAxis = {};
	uMATH::vec3f_t RelativeXAxis = {};
	uMATH::vec3f_t RelativeYAxis = {};

	void LookAtMouse(double XOffset, double YOffset);
	void Move(GLFWwindow *Window);
};

#endif

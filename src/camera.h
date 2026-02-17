#ifndef VOID_CAMERA_H
#define VOID_CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

struct camera_info_t
{
	// Fixed values - changed by system code
	float Sensitivity = 0.1f;
	float Speed = 0.0f;
	float Yaw = -90.0f;
	float Pitch = 0.0f;

	float h_fov;
	float focal_length;
	float aspect_ratio;
	float near_plane_distance;
	float far_plane_distance;
	glm::vec3 Eye = {0.0f, 0.0f, -1.0f};
	glm::vec3 UpAxis = {0.0f, 1.0f, 0.0f};

	// Runtime values - changed by user actions
	glm::vec3 Position = {0.0f, 0.0f, 3.0f};
	glm::vec3 RelativeXAxis = {};
	glm::vec3 RelativeYAxis = {};

	void LookAtMouse(double XOffset, double YOffset);
	void Move(GLFWwindow* Window);
};

#endif

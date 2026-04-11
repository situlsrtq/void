#ifndef VOID_CAMERA_H
#define VOID_CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

struct camera_info_t
{
	// Fixed values - changed by system code
	float sensitivity = 0.1f;
	float speed = 0.0f;
	float yaw = -90.0f;
	float pitch = 0.0f;

	float h_fov;
	float focal_length;
	float aspect_ratio;
	float near_plane_distance;
	float far_plane_distance;
	glm::vec3 eye = {0.0f, 0.0f, -1.0f};
	glm::vec3 up_axis = {0.0f, 1.0f, 0.0f};

	// Runtime values - changed by user actions
	glm::vec3 position = {0.0f, 0.0f, 3.0f};
	glm::vec3 rel_x_axis = {};
	glm::vec3 rel_y_axis = {};

	void look_at_mouse(double xoffset, double yoffset);
	void move(GLFWwindow* window);
};

#endif

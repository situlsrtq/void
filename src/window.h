#ifndef VOID_WINDOW_H
#define VOID_WINDOW_H

#include <imgui/imgui.h>
#include <glm/mat4x4.hpp>
#include "scene.h"
#include "shader.h"
#include "u_hash.h"
#include "renderpass.h"
#include "camera.h"

#define EMODE_VIEW 0
#define EMODE_GEOMETRY 1
#define EMODE_LIGHTS 2

// Monolithic object an unfortunate consequence of using GLFW - future improvements could write a better base layer
// for cross-platform windowing and simplify this object
struct window_handler_t
{
	int width;
	int height;
	float delta_time;
	float prev_frame_time;
	float frame_time_ms;
	uint16_t first_camera_move;
	uint16_t editor_mode;
	bool active_selection;
	bool reload_shaders;
	bool should_exit;
	double prev_mouse_x;
	double prev_mouse_y;

	shader_program_t main_shader;
	shader_program_t post_shader;
	shader_program_t pick_shader;
	fb_hdr_t hdr_pass_fb;
	fb_mpick_t pick_pass_fb;
	camera_info_t camera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 inverse_vp;
	primitive_create_info_t active; // for fucking with primitives, later
	scene_info_t scene;
	hash_table_t hash_table;
	ImGuiIO im_io;
};

window_handler_t* init_window_handler(float ScreenX, float ScreenY, linear_arena_t* string_arena);

#endif

#ifndef VOID_WINDOW_H
#define VOID_WINDOW_H

#include <imgui/imgui.h>
#include <glm/mat4x4.hpp>
#include "scene.h"
#include "shader.h"
#include "renderpass.h"
#include "camera.h"

#define EMODE_VIEW 0
#define EMODE_GEOMETRY 1
#define EMODE_LIGHTS 2

// Monolithic object an unfortunate consequence of using GLFW - future improvements could write a better base layer
// for cross-platform windowing and simplify this object
struct window_handler_t
{
	int Width;
	int Height;
	float DeltaTime;
	float PrevFrameTime;
	float FrameTimeMS;
	uint16_t FirstCameraMove;
	uint16_t EditorMode;
	bool ActiveSelection;
	bool ReloadShaders;
	bool ShouldExit;
	double PrevMouseX;
	double PrevMouseY;

	shader_program_t MainShader;
	shader_program_t PostShader;
	shader_program_t PickShader;
	fb_hdr_t HDRPass;
	fb_mpick_t PickPass;
	mbox_camera_t Camera;
	glm::mat4 View;
	glm::mat4 Projection;
	primitive_create_info_t Active; // for fucking with primitives, later
	scene_info_t Scene;
	ImGuiIO ImIO;
};

window_handler_t* InitWindowHandler(float ScreenX, float ScreenY);

#endif

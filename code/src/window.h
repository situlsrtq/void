#ifndef MBOX_WINDOW_H
#define MBOX_WINDOW_H


#include "u_math.h"
#include "u_mem.h"
#include "shader.h"
#include "picking.h"
#include "camera.h"


#define EMODE_VIEW 0
#define EMODE_GEOMETRY 1
#define EMODE_LIGHTS 2


struct window_handler_t
{
	int Width;
	int Height;
	float DeltaTime;
	float PrevFrameTime;
	uint16_t FirstCameraMove;
	uint16_t EditorMode;
	unsigned int ActiveSelection;
	double PrevMouseX;
	double PrevMouseY;

	shader_t MainShader;
	shader_t PickShader;
	fb_mpick_t PickPass;
	mbox_camera_t Camera;
	uMATH::mat4f_t View;
	uMATH::mat4f_t Projection;
	geometry_create_info_t Active;
	geometry_state_t GeometryObjects;
};


window_handler_t* InitWindowHandler(float ScreenX, float ScreenY);


#endif

#include "window.h"


window_handler_t* InitWindowHandler(float ScreenX, float ScreenY)
{
	window_handler_t *res = (window_handler_t*)calloc(1, sizeof(window_handler_t));
	if (!res)
	{
		printf("System: window handler failed to allocate\n");
		return 0x0;
	}

	res->DeltaTime = 0.0f;
	res->PrevFrameTime = 0.0f;
	res->FirstCameraMove = 1;
	res->EditorMode = EMODE_VIEW;
	res->ActiveSelection = 0;
	res->PrevMouseX = ScreenX / 2.0f;
	res->PrevMouseY = ScreenY / 2.0f;

	res->Camera.Sensitivity = 0.1f;
	res->Camera.Speed = 0.0f;
	res->Camera.Yaw = -90.0f;
	res->Camera.Pitch = 0.0f;
	res->Camera.Position = {0.0f, 0.0f, 3.0f};
	res->Camera.Eye = {0.0f,0.0f,-1.0f};
	res->Camera.UpAxis = {0.0f,1.0f,0.0f};

	res->Active.Scale = 1.0f;
	res->Active.Intensity = 0.5f;
	res->Active.Color = {1.0f, 0.5f, 0.31f};

	return res;
}
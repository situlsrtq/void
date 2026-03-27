#include "window.h"

window_handler_t* init_window_handler(float ScreenX, float ScreenY, linear_arena_t* string_arena)
{
	window_handler_t* res = (window_handler_t*)calloc(1, sizeof(window_handler_t));
	if(!res)
	{
		printf("System: window handler failed to allocate\n");
		return 0x0;
	}

	res->width = ScreenX;
	res->height = ScreenY;
	res->first_camera_move = 1;
	res->editor_mode = EMODE_VIEW;
	res->active_selection = false;
	res->reload_shaders = false;
	res->should_exit = false;
	res->prev_mouse_x = ScreenX / 2.0f;
	res->prev_mouse_y = ScreenY / 2.0f;

	res->camera.Sensitivity = 0.1f;
	res->camera.Yaw = -90.0f;
	res->camera.Position = {0.0f, 0.0f, 3.0f};
	res->camera.Eye = {0.0f, 0.0f, -1.0f};
	res->camera.UpAxis = {0.0f, 1.0f, 0.0f};

	res->active.color = {1.0f, 0.5f, 0.31f};

	int success = res->hash_table.init(string_arena);
	if(success == EXIT_FAILURE)
	{
		printf("System: hash table failed to allocate\n");
		UTIL::Free(res);
		return res;
	}

	return res;
}

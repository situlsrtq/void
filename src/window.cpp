#include "window.h"

window_handler_t* init_window_handler(float ScreenX, float ScreenY, linear_arena_t* persistent_arena,
				      linear_arena_t* string_arena)
{
	u64 handle;
	int alloc_res = arena_alloc(persistent_arena, &handle, sizeof(window_handler_t));
	if(!alloc_res)
	{
		printf("System: window handler failed to allocate\n");
		return 0x0;
	}
	window_handler_t* res = (window_handler_t*)pointer_from_arena(persistent_arena, handle);

	res->width = ScreenX;
	res->height = ScreenY;
	res->first_camera_move = 1;
	res->editor_mode = EMODE_VIEW;
	res->active_selection = false;
	res->reload_shaders = false;
	res->should_exit = false;
	res->prev_mouse_x = ScreenX / 2.0f;
	res->prev_mouse_y = ScreenY / 2.0f;

	res->camera.sensitivity = 0.1f;
	res->camera.yaw = -90.0f;
	res->camera.position = {0.0f, 0.0f, 3.0f};
	res->camera.eye = {0.0f, 0.0f, -1.0f};
	res->camera.up_axis = {0.0f, 1.0f, 0.0f};

	res->active.color = {1.0f, 0.5f, 0.31f};

	int success = res->hash_table.init(string_arena);
	if(success == EXIT_FAILURE)
	{
		printf("System: hash table failed to allocate\n");
		return 0x0;
	}

	return res;
}

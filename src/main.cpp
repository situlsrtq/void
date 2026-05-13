#include "main.h"

// TODO: Get rid of runtime path discovery in release builds

#ifdef DEBUG
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
				const GLchar* message, const void* userParam)
{
	(void)source;
	(void)userParam;
	(void)length;
	(void)id;

	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}
#endif

// Uniform globals. Garbage doodoo trash, will go when renderer transitions to visbuffer and indirect/bindless

unsigned int model_uni;
unsigned int minvt_uni;
unsigned int view_uni;
unsigned int viewpos_uni;
unsigned int projection_uni;
unsigned int light_dir_uni;
unsigned int objcolor_uni;
unsigned int light_color_uni;
unsigned int ambistrgth_uni;
unsigned int exposure_uni;
float exposure_val;

// Input globals

struct key_state_t
{
	int keys[GLFW_KEY_LAST];
};

struct mouse_state_t
{
	int buttons[GLFW_MOUSE_BUTTON_LAST];
};

key_state_t curr_key_inputs;
key_state_t past_key_inputs;
mouse_state_t curr_mbutton_inputs;
mouse_state_t past_mbutton_inputs;

linear_arena_t string_arena;
linear_arena_t persistent_arena;
linear_arena_t scratch_arena;

int main(void)
{
	// Intialize memory systems

	int res = PAL::get_path(g_PathBuffer_r, VOID_PATH_MAX);
	if(res != EXIT_SUCCESS)
	{
		printf("PAL: Failed to initialize file path\n");
	}

	// Persistent, string specific storage
	res = string_arena.init(V_MIB(8));
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main thread string memory\n");
	}

	// Storage for data that persists between frames. Never cleared by default
	res = persistent_arena.init(V_MIB(50));
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main thread persistent memory\n");
	}

	// Storage for per-frame data. Cleared at the end of every frame
	res = scratch_arena.init(V_MIB(4));
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main thread scratch space memory\n");
	}

	const char* res1 = "res/sponza.glb";
	const char* res2 = "res/test_bench.glb";
	// Drop the null terminator on OSPath intentionally, since it will always be concatenated with
	// paths. hacky stupid shit, will disappear when a real file picker is implemented
	size_t pathlen = strlen(g_OSPath_r);
	size_t filelen = strlen(res1) + 1;
	size_t f2len = strlen(res2) + 1;

	u64 f1_handle;
	res = arena_alloc(&string_arena, &f1_handle, pathlen + filelen);
	if(res == EXIT_FAILURE)
	{
		printf("Scene: file 1 alloc failed\n");
		return EXIT_FAILURE;
	}
	char* scene_file1 = (char*)pointer_from_arena(&string_arena, f1_handle);
	memcpy(scene_file1, g_OSPath_r, pathlen);
	memcpy(scene_file1 + pathlen, res1, filelen);

	u64 f2_handle;
	res = arena_alloc(&string_arena, &f2_handle, pathlen + f2len);
	if(res == EXIT_FAILURE)
	{
		printf("Scene: file 2 alloc failed\n");
		return EXIT_FAILURE;
	}
	char* scene_file2 = (char*)pointer_from_arena(&string_arena, f2_handle);
	memcpy(scene_file2, g_OSPath_r, pathlen);
	memcpy(scene_file2 + pathlen, res2, f2len);

	// Initialize Core Systems

	if(!glfwInit())
	{
		return EXIT_FAILURE;
	}

	const char* GLSLVersion = "#version 460";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// TODO: use glfwSetWindowAttrib() to allow for one-time resizing operations as menu
	// selections
	glfwWindowHint(GLFW_RESIZABLE, true);

	GLFWwindow* window = glfwCreateWindow(SCREEN_X_DIM_DEFAULT, SCREEN_Y_DIM_DEFAULT, "void", 0, 0);
	if(!window)
	{
		printf("GLFW: Failed to create window\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	// /!\ gladLoadGLLoader() overwrites all gl functions, can only be called after successfully
	// setting a current context

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD: Failed getting function pointers\n");
		return EXIT_FAILURE;
	}

	GPU_CONTEXT;
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* card = glGetString(GL_RENDERER);

	printf("GPU: %s %s\n", vendor, card);

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

	window_handler_t* win_hnd = init_window_handler(SCREEN_X_DIM_DEFAULT, SCREEN_Y_DIM_DEFAULT, &persistent_arena,
							&string_arena);
	if(!win_hnd)
	{
		printf("System: Could not allocate core window handler\n");
		return EXIT_FAILURE;
	}
	glfwSetWindowUserPointer(window, (void*)win_hnd);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, frame_resize_callback);

	// Initialize ImGui Context

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	win_hnd->im_io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	bool b_res = false;
	b_res = ImGui_ImplGlfw_InitForOpenGL(window, true);
	if(!b_res)
	{
		printf("System: Could not initialize imgui context for GLFW\n");
		return EXIT_FAILURE;
	}
	b_res = ImGui_ImplOpenGL3_Init(GLSLVersion);
	if(!b_res)
	{
		printf("System: Could not initialize imgui context for OpenGL\n");
		return EXIT_FAILURE;
	}

	// Initialize Render Data Structures

	vertex_buffer_info_t vbuffer_state = {};
	glCreateBuffers(VOID_VBUFCOUNT_FMT, vbuffer_state.vbuffer_array);
	glNamedBufferStorage(vbuffer_state.vbuffer_array[INDEX_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(vbuffer_state.vbuffer_array[POS_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(vbuffer_state.vbuffer_array[NORM_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(vbuffer_state.vbuffer_array[TAN_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(vbuffer_state.vbuffer_array[TEX_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);

	unsigned int vao;
	glCreateVertexArrays(1, &vao);

	glVertexArrayElementBuffer(vao, vbuffer_state.vbuffer_array[INDEX_BUFFER]);	    // sizeof(vec3)
	glVertexArrayVertexBuffer(vao, 0, vbuffer_state.vbuffer_array[POS_BUFFER], 0, 12);  // sizeof(vec3)
	glVertexArrayVertexBuffer(vao, 1, vbuffer_state.vbuffer_array[NORM_BUFFER], 0, 12); // sizeof(vec3)
	glVertexArrayVertexBuffer(vao, 2, vbuffer_state.vbuffer_array[TAN_BUFFER], 0, 16);  // sizeof(vec3)
	glVertexArrayVertexBuffer(vao, 3, vbuffer_state.vbuffer_array[TEX_BUFFER], 0, 8);   // sizeof(vec3)

	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0);

	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 1, 1);

	glEnableVertexArrayAttrib(vao, 2);
	glVertexArrayAttribFormat(vao, 2, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 2, 2);

	glEnableVertexArrayAttrib(vao, 3);
	glVertexArrayAttribFormat(vao, 3, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 3, 3);

	res = glb_import(scene_file1, win_hnd, &vao, &vbuffer_state, VOID_TEX_COUNT);
	if(res == EXIT_FAILURE)
	{
		printf("System: Could not load glb file: %s\n", scene_file1);
		return EXIT_FAILURE;
	}
	res = glb_import(scene_file2, win_hnd, &vao, &vbuffer_state, VOID_TEX_COUNT);
	if(res == EXIT_FAILURE)
	{
		printf("System: Could not load glb file: %s\n", scene_file2);
		return EXIT_FAILURE;
	}

	// TODO: switch to glDrawElementsIndirectCommand
	// /!\ remember to make changes where the draw calls actually happen too

	glBindVertexArray(0);

	// Initialize Render passes

	// Main pass

	res = win_hnd->hdr_pass_fb.Init(win_hnd->width, win_hnd->height);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main (HDR) pass framebuffer\n");
		return EXIT_FAILURE;
	}

	shader_info_t main_pass_params = {};
	res = main_pass_params.Init("shaders/main.vert", 0, 0, 0, "shaders/main.frag", 0);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main pass shader parameters\n");
		return EXIT_FAILURE;
	}
	res = win_hnd->main_shader.Create(main_pass_params);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main pass shaders\n");
		return EXIT_FAILURE;
	}

	model_uni = glGetUniformLocation(win_hnd->main_shader.ID, "model");
	minvt_uni = glGetUniformLocation(win_hnd->main_shader.ID, "minvt");
	view_uni = glGetUniformLocation(win_hnd->main_shader.ID, "view");
	viewpos_uni = glGetUniformLocation(win_hnd->main_shader.ID, "viewpos");
	projection_uni = glGetUniformLocation(win_hnd->main_shader.ID, "projection");
	light_dir_uni = glGetUniformLocation(win_hnd->main_shader.ID, "lightdir");
	objcolor_uni = glGetUniformLocation(win_hnd->main_shader.ID, "objcolor");
	light_color_uni = glGetUniformLocation(win_hnd->main_shader.ID, "lightcolor");
	ambistrgth_uni = glGetUniformLocation(win_hnd->main_shader.ID, "ambientstrength");

	// Post pass

	shader_info_t post_pass_params = {};
	res = post_pass_params.Init("shaders/post.vert", 0, 0, 0, "shaders/post.frag", 0);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize post pass shader parameters\n");
		return EXIT_FAILURE;
	}
	res = win_hnd->post_shader.Create(post_pass_params);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize post pass shaders\n");
		return EXIT_FAILURE;
	}

	exposure_uni = glGetUniformLocation(win_hnd->post_shader.ID, "exposure");

	// Visbuffer pass

	res = win_hnd->pick_pass_fb.Init(win_hnd->width, win_hnd->height);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize pick pass framebuffer\n");
		return EXIT_FAILURE;
	}

	shader_info_t pick_pass_params = {};
	res = pick_pass_params.Init("shaders/pick.vert", 0, 0, 0, "shaders/pick.frag", 0);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize pick shader parameters\n");
		return EXIT_FAILURE;
	}
	res = win_hnd->pick_shader.Create(pick_pass_params);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize pick pass shaders\n");
		return EXIT_FAILURE;
	}

	unsigned int pickingmodel_uni = glGetUniformLocation(win_hnd->pick_shader.ID, "model");
	unsigned int pickingview_uni = glGetUniformLocation(win_hnd->pick_shader.ID, "view");
	unsigned int pickingprojection_uni = glGetUniformLocation(win_hnd->pick_shader.ID, "projection");
	unsigned int pickingindex_uni = glGetUniformLocation(win_hnd->pick_shader.ID, "index");
	unsigned int pickingtype_uni = glGetUniformLocation(win_hnd->pick_shader.ID, "type");

	// Initialize first-frame data

	win_hnd->camera.h_fov = glm::radians(VOID_HFOV_DEFAULT);
	win_hnd->camera.focal_length = SCREEN_Y_DIM_DEFAULT / (2.0f * tanf(win_hnd->camera.h_fov / 2.0f));
	win_hnd->camera.aspect_ratio = SCREEN_X_DIM_DEFAULT / SCREEN_Y_DIM_DEFAULT;
	win_hnd->camera.near_plane_distance = 0.1f;
	win_hnd->camera.far_plane_distance = 100.0f;
	win_hnd->projection = glm::perspective(win_hnd->camera.h_fov, win_hnd->camera.aspect_ratio,
					       win_hnd->camera.near_plane_distance, win_hnd->camera.far_plane_distance);

	glm::mat4 model = {};
	glm::vec3 light_dir = {1.2f, -10.0f, 2.0f};
	glm::vec3 light_scale = {0.2f, 0.2f, 0.2f};

	float frame_start_time = 0;
	float frame_end_time = 0;
	exposure_val = 0.5;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	int render_mode = GL_TRIANGLES;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Frame loop

	bool help_window = false;
	bool post_window = false;
	bool demo_window = false;
	command_buffer_t command_buffer = {};

	while(!glfwWindowShouldClose(window))
	{
		frame_start_time = glfwGetTime();

		// Handle user input

		glfwPollEvents();
		win_hnd->im_io = ImGui::GetIO(); // sometimes imgui will want to capture the mouse or keyboard, so check
						 // before processing input
		process_input(win_hnd, window);

		// UI framegen

		gen_interface_elements(win_hnd, &help_window, &post_window, &demo_window);

		// Command buffers and culling

		command_buffer_frame_start(&scratch_arena, &command_buffer, PROGRAM_MAX_OBJECTS);

		dual_grid_frustum_cull(win_hnd->dual_grid, &command_buffer, win_hnd->inverse_vp);

		command_buffer_frame_end(&command_buffer);

		// Render passes

		glBindVertexArray(vao);

		// TODO: VISBUFFER. Can also be used for mouse picking

		{
			GPU_ZONE("Visbuffer");
			win_hnd->pick_pass_fb.Bind();

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			win_hnd->pick_shader.Use();

			glUniformMatrix4fv(pickingprojection_uni, 1, GL_FALSE, glm::value_ptr(win_hnd->projection));
			glUniformMatrix4fv(pickingview_uni, 1, GL_FALSE, glm::value_ptr(win_hnd->view));

			u32 id;
			for(unsigned int i = 0; i < command_buffer.curr_command_count; i++)
			{
				id = command_buffer.command_list[i].node_id;
				node_create_info_t node = win_hnd->scene.node[id];

				glUniform1f(pickingindex_uni, float(node.node_id));
				glUniform1f(pickingtype_uni, float(1));

				glUniformMatrix4fv(pickingmodel_uni, 1, GL_FALSE,
						   glm::value_ptr(win_hnd->scene.model_matrix[node.node_id]));

				mesh_info_t mesh = win_hnd->scene.mesh[node.mesh_index];
				for(uint32_t t = 0; t < mesh.size; t++)
				{
					primitive_create_info_t prim = win_hnd->scene.prim[mesh.base_index + t];
					if(prim.index_info.index_count)
					{
						glDrawElementsBaseVertex(render_mode, prim.index_info.index_count,
									 prim.index_info.index_type,
									 (void*)prim.index_info.ebo_byte_offset,
									 prim.vertex_info.vertex_offset);
					}
					else
					{
						glDrawArrays(render_mode, prim.vertex_info.vertex_offset,
							     prim.vertex_info.vattr_count);
					}
				}
			}
		} // SCOPING FOR TRACY

		// Object Geometry

		{
			GPU_ZONE("Geometry");
			win_hnd->hdr_pass_fb.Bind();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			win_hnd->main_shader.Use();

			glUniform1f(ambistrgth_uni, 0.1f);
			glUniform3f(light_color_uni, 1.0f, 1.0f, 1.0f);
			glUniform3f(light_dir_uni, light_dir.x, light_dir.y, light_dir.z);

			glUniformMatrix4fv(projection_uni, 1, GL_FALSE, glm::value_ptr(win_hnd->projection));

			glUniformMatrix4fv(view_uni, 1, GL_FALSE, glm::value_ptr(win_hnd->view));
			glUniform3f(viewpos_uni, win_hnd->camera.position.x, win_hnd->camera.position.y,
				    win_hnd->camera.position.z);

			u32 id;
			for(unsigned int i = 0; i < command_buffer.curr_command_count; i++)
			{
				id = command_buffer.command_list[i].node_id;
				node_create_info_t node = win_hnd->scene.node[id];

				glUniformMatrix4fv(model_uni, 1, GL_FALSE,
						   glm::value_ptr(win_hnd->scene.model_matrix[node.node_id]));

				mesh_info_t mesh = win_hnd->scene.mesh[node.mesh_index];
				for(uint32_t t = 0; t < mesh.size; t++)
				{
					primitive_create_info_t prim = win_hnd->scene.prim[mesh.base_index + t];

					glUniformMatrix3fv(minvt_uni, 1, GL_FALSE,
							   glm::value_ptr(prim.model_inv_trans));
					glUniform3fv(objcolor_uni, 1, glm::value_ptr(prim.color));

					glBindTextureUnit(0, prim.texture_info.tex_array[0]);
					glBindTextureUnit(1, prim.texture_info.tex_array[1]);
					glBindTextureUnit(2, prim.texture_info.tex_array[2]);

					if(prim.index_info.index_count)
					{
						glDrawElementsBaseVertex(render_mode, prim.index_info.index_count,
									 prim.index_info.index_type,
									 (void*)prim.index_info.ebo_byte_offset,
									 prim.vertex_info.vertex_offset);
					}
					else
					{
						glDrawArrays(render_mode, prim.vertex_info.vertex_offset,
							     prim.vertex_info.vattr_count);
					}
				}
			}
		} // SCOPING FOR TRACY

		if(win_hnd->active_selection)
		{
			/*
			win_hnd->Active.ComposeModelM4();
			glUniformMatrix4fv(model_uni, 1, GL_FALSE, &win_hnd->Active.Model.m[0][0]);
			glUniform3fv(objcolor_uni, 1, &win_hnd->Active.Color.x);
			glDrawElements(RenderMode, indexcount, indextype, (void*)0);
			*/
		}

		// Light Geometry

		glUniform1f(ambistrgth_uni, 1.0f);
		glUniform3f(objcolor_uni, 1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0f);
		model = glm::scale(model, light_scale);
		glUniformMatrix4fv(model_uni, 1, GL_FALSE, glm::value_ptr(model));

		/*
		if(win_hnd->GeometryObjects.IndexCount[0])
		{
			glDrawElementsBaseVertex(RenderMode, win_hnd->GeometryObjects.IndexCount[0],
			    win_hnd->GeometryObjects.IndexType[0],
			    (void*)win_hnd->GeometryObjects.ByteOffsetEBO[0],
			    win_hnd->GeometryObjects.OffsetVBO[0]);
		}
		else
		{
			glDrawArrays(RenderMode, win_hnd->GeometryObjects.OffsetVBO[0],
		win_hnd->GeometryObjects.VAttrCount[0]);
		}
		*/

		// Blit from HDR to linear normal quad for post-processing, parse inter-frame data

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, win_hnd->hdr_pass_fb.ColorBuffer);

		win_hnd->post_shader.Use();
		glUniform1f(exposure_uni, float(exposure_val));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Attributes hard coded in vertex shader

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		frame_end_time = glfwGetTime();
		win_hnd->frame_time_ms = (frame_end_time - frame_start_time) * 1000.0f;

		glfwSwapBuffers(window);
		GPU_COLLECT;

		scratch_arena.reset();

		win_hnd->delta_time = frame_end_time - win_hnd->prev_frame_time;
		win_hnd->prev_frame_time = frame_end_time;
		FRAME_MARK;
	}

	// Free resources and exit - not technically necessary when this is the end of the program,
	// but future-proofs for mutlithreading or other integrations

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return EXIT_SUCCESS;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)window, (void)mods, (void)scancode;

	if((key == GLFW_KEY_UNKNOWN) || (action == GLFW_REPEAT))
	{
		return;
	}
	past_key_inputs.keys[key] = curr_key_inputs.keys[key];
	curr_key_inputs.keys[key] = action;
}

int is_key(int key)
{
	return curr_key_inputs.keys[key];
}

int was_key(int key)
{
	return past_key_inputs.keys[key];
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	(void)window, (void)mods;

	past_mbutton_inputs.buttons[button] = curr_mbutton_inputs.buttons[button];
	curr_mbutton_inputs.buttons[button] = action;
}

int is_mbutton(int button)
{
	return curr_mbutton_inputs.buttons[button];
}

int was_mbutton(int button)
{
	return past_mbutton_inputs.buttons[button];
}

void frame_resize_callback(GLFWwindow* Window, int new_screen_width, int new_screen_height)
{
	window_handler_t* win_hnd = (window_handler_t*)glfwGetWindowUserPointer(Window);

	win_hnd->width = new_screen_width;
	win_hnd->height = new_screen_height;

	// Also resize camera frustum and all existing framebuffers
	win_hnd->camera.focal_length = new_screen_height / (2.0f * tanf(win_hnd->camera.h_fov / 2.0f));
	win_hnd->camera.aspect_ratio = (float)new_screen_width / (float)new_screen_height;
	win_hnd->projection = glm::perspective(win_hnd->camera.h_fov, win_hnd->camera.aspect_ratio,
					       win_hnd->camera.near_plane_distance, win_hnd->camera.far_plane_distance);
	win_hnd->hdr_pass_fb.Release();
	win_hnd->hdr_pass_fb.Init(new_screen_width, new_screen_height);
	win_hnd->pick_pass_fb.Release();
	win_hnd->pick_pass_fb.Init(new_screen_width, new_screen_height);

	glViewport(0, 0, new_screen_width, new_screen_height);
}

void mouse_pos_callback(GLFWwindow* Window, double mx, double my)
{
	window_handler_t* win_hnd = (window_handler_t*)glfwGetWindowUserPointer(Window);

	// Check if the UI should be pulling focus
	if(win_hnd->im_io.WantCaptureMouse)
	{
		return;
	}

	if(win_hnd->first_camera_move)
	{
		win_hnd->prev_mouse_x = mx;
		win_hnd->prev_mouse_y = my;
		win_hnd->first_camera_move = 0;
	}

	float xoffset = mx - win_hnd->prev_mouse_x;
	float yoffset = win_hnd->prev_mouse_y - my;

	win_hnd->prev_mouse_x = mx;
	win_hnd->prev_mouse_y = my;

	if(is_mbutton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		win_hnd->camera.look_at_mouse(xoffset, yoffset);
	}
}

void process_input(window_handler_t* win_hnd, GLFWwindow* window)
{
	// Check if the UI should be pulling focus
	if(win_hnd->im_io.WantCaptureKeyboard)
	{
		return;
	}

	win_hnd->camera.speed = 2.5f * win_hnd->delta_time;
	win_hnd->camera.rel_x_axis = glm::normalize(glm::cross(win_hnd->camera.eye, win_hnd->camera.up_axis));
	win_hnd->camera.rel_y_axis = glm::normalize(glm::cross(win_hnd->camera.rel_x_axis, win_hnd->camera.eye));
	win_hnd->camera.move(window);
	win_hnd->view = glm::lookAt(win_hnd->camera.position, win_hnd->camera.position + win_hnd->camera.eye,
				    win_hnd->camera.up_axis);
	win_hnd->inverse_vp = glm::inverse(win_hnd->projection * win_hnd->view);

	if(is_key(GLFW_KEY_ESCAPE) == KEY_PRESS || win_hnd->should_exit)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if(is_key(GLFW_KEY_Q) == KEY_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if(is_key(GLFW_KEY_E) == KEY_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if(is_key(GLFW_KEY_P) == KEY_RELEASE)
	{
		if((was_key(GLFW_KEY_P) == KEY_PRESS) || win_hnd->reload_shaders)
		{
			win_hnd->main_shader.Rebuild();
			model_uni = glGetUniformLocation(win_hnd->main_shader.ID, "model");
			minvt_uni = glGetUniformLocation(win_hnd->main_shader.ID, "minvt");
			view_uni = glGetUniformLocation(win_hnd->main_shader.ID, "view");
			viewpos_uni = glGetUniformLocation(win_hnd->main_shader.ID, "viewpos");
			projection_uni = glGetUniformLocation(win_hnd->main_shader.ID, "projection");
			light_dir_uni = glGetUniformLocation(win_hnd->main_shader.ID, "lightdir");
			objcolor_uni = glGetUniformLocation(win_hnd->main_shader.ID, "objcolor");
			light_color_uni = glGetUniformLocation(win_hnd->main_shader.ID, "lightcolor");
			ambistrgth_uni = glGetUniformLocation(win_hnd->main_shader.ID, "ambientstrength");

			win_hnd->reload_shaders = false;
		}
	}
	if(is_key(GLFW_KEY_N) == GLFW_RELEASE)
	{
		if(was_key(GLFW_KEY_N) == KEY_PRESS)
		{
			/*
			uMATH::vec3f_t p = { 0.0f,0.0f,4.5f };
			win_hnd->Active.Model = win_hnd->View;
			uMATH::Translate(&win_hnd->Active.Model, p);
			win_hnd->Active.Model = uMATH::InverseM4(win_hnd->Active.Model);
			win_hnd->Active.DecomposeModelM4();
			win_hnd->Active.New = false;
			win_hnd->ActiveSelection = true;
			*/
		}
	}

	// Have to separately check if the UI should be pulling mouse button inputs, as they aren't
	// tracked by WantCaptureKeyboard
	if(win_hnd->im_io.WantCaptureMouse)
	{
		return;
	}

	if(is_mbutton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		if(was_mbutton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			/*
			texel_info_t res = win_hnd->PickPass.GetInfo((uint32_t)win_hnd->PrevMouseX,
								    (uint32_t)(win_hnd->Height - win_hnd->PrevMouseY));
			if(win_hnd->ActiveSelection && win_hnd->Active.Deleted != true)
			{
				win_hnd->Active.ComposeModelM4();
				win_hnd->GeometryObjects.Alloc(win_hnd->Active);
				win_hnd->ActiveSelection = false;
			}
			if (res.ID > 0)
			{
				win_hnd->Active.Model = win_hnd->GeometryObjects.Model[(int)res.ID -
			1]; win_hnd->Active.Color = win_hnd->GeometryObjects.Color[(int)res.ID - 1];
				win_hnd->Active.DecomposeModelM4();
				win_hnd->GeometryObjects.Free((int)res.ID - 1);
				win_hnd->ActiveSelection = true;
			}
			*/
		}
	}

	if(is_mbutton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if(was_mbutton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	if(is_mbutton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		if(was_mbutton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void gen_interface_elements(window_handler_t* win_hnd, bool* HelpWindow, bool* PostWindow, bool* DemoWindow)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if(win_hnd->active_selection)
	{
		ImGui::Begin("Object Parameters");

		/*
		ImGui::Spacing();
		ImGui::SliderFloat("Scale", &win_hnd->Active.Scale, 0.1f, 2.5f);
		ImGui::Spacing();
		ImGui::Text("Rotation");
		ImGui::SliderFloat("Angle", &win_hnd->Active.RotationAngle, 0.0f, 180.0f);
		ImGui::SliderFloat("rX", &win_hnd->Active.RotationAxis.x, 0.0f, 1.0f);
		ImGui::SliderFloat("rY", &win_hnd->Active.RotationAxis.y, 0.0f, 1.0f);
		ImGui::SliderFloat("rZ", &win_hnd->Active.RotationAxis.z, 0.0f, 1.0f);
		ImGui::Spacing();
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &win_hnd->Active.Position.x, -15.0f, 15.0f);
		ImGui::SliderFloat("Y", &win_hnd->Active.Position.y, -15.0f, 15.0f);
		ImGui::SliderFloat("Z", &win_hnd->Active.Position.z, -15.0f, 15.0f);
		ImGui::Spacing();
		ImGui::ColorEdit3("Color", (float*)&win_hnd->Active.Color);
		ImGui::Spacing();
		if (ImGui::Button("Delete Object"))
		{
			win_hnd->Active.Deleted = true;
			win_hnd->ActiveSelection = false;
		}
		*/

		ImGui::End();
	}
	if(*PostWindow)
	{
		ImGui::Begin("Post-Processing");

		ImGui::Spacing();
		ImGui::SliderFloat("Exposure", &exposure_val, 0.0f, 5.0f);
		ImGui::Spacing();
		if(ImGui::Button("Close"))
		{
			*PostWindow = false;
		}

		ImGui::End();
	}
	if(*HelpWindow)
	{
		ImGui::Begin("Help");

		ImGui::Text("Use WASD keys to move left, right, forward, and backward");
		ImGui::Spacing();
		ImGui::Text("Use LShift to move down. Use Spacebar to move up");
		ImGui::Spacing();
		ImGui::Text("Hold Right Mouse button to control camera with mouse movement");
		ImGui::Spacing();
		ImGui::Text("UI elements capture mouse commands when hovered.");
		ImGui::Text("Move the mouse outside the menu to interact with the scene");
		ImGui::Spacing();
		if(ImGui::Button("Close"))
		{
			*HelpWindow = false;
		}

		ImGui::End();
	}
#ifdef DEBUG
	else if(*DemoWindow)
	{
		ImGui::ShowDemoWindow(DemoWindow);
	}
#endif
	ImGui::Begin("Scene Controls");

	ImGui::Spacing();
	if(ImGui::Button("Post-Processing"))
	{
		*PostWindow = true;
	}
	ImGui::Spacing();
	if(ImGui::Button("Reload Shaders (p)"))
	{
		win_hnd->reload_shaders = true;
	}
	ImGui::Spacing();
	if(ImGui::Button("Help"))
	{
		*HelpWindow = true;
	}
#ifdef DEBUG
	ImGui::SameLine();
	if(ImGui::Button("Show Demo Window"))
	{
		*DemoWindow = true;
	}
#endif
	ImGui::Spacing();
	if(ImGui::Button("Exit (esc)"))
	{
		win_hnd->should_exit = true;
	}
	ImGui::Spacing();
	ImGui::Text("CPU Frame time: %.2f ms", win_hnd->frame_time_ms);
	ImGui::Spacing();
	ImGui::Text("Frame present time: %.2f ms", win_hnd->delta_time * 1000.0f);

	ImGui::End();
}

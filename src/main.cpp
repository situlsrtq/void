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

uint8_t NKeyWasDown;
uint8_t RKeyWasDown;
uint8_t PKeyWasDown;
uint8_t LMouseWasDown;
uint8_t RMouseWasDown;

unsigned int model_uni;
unsigned int minvt_uni;
unsigned int view_uni;
unsigned int viewpos_uni;
unsigned int projection_uni;
unsigned int lightpos_uni;
unsigned int objcolor_uni;
unsigned int lightcolor_uni;
unsigned int ambistrgth_uni;
unsigned int exposure_uni;
float exposure_val;

int main(void)
{
	int res = PAL::GetPath(g_PathBuffer_r, VOID_PATH_MAX);
	if(res != EXIT_SUCCESS)
	{
		printf("PAL: Failed to initialize file path\n");
	}

	g_test_table = (hash_table_t*)calloc(1, sizeof(hash_table_t));
	res = g_test_table->StringArena.Init(V_MIB(4), V_MIB(1));
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize resource hash table\n");
	}

	// TODO: per-thread string memory system, to be sized based on thread's need. Rendering
	// manager will be heaviest user

	void* ResourceStringMem = (char*)malloc(V_MIB(4));
	if(!ResourceStringMem)
	{
		return EXIT_FAILURE;
	}

	char* CurrStringMem = (char*)ResourceStringMem;
	const char* ResFile = "res/sponza.glb";
	const char* Res2 = "res/chess.glb";
	const char* UIFile = "config/imgui.ini";
	// Drop the null terminator on OSPath intentionally, since it will be concatenated with
	// paths. hacky stupid shit, will not last
	size_t pathlen = strlen(g_OSPath_r);
	size_t filelen = strlen(ResFile) + 1;
	size_t configlen = strlen(UIFile) + 1;

	memcpy(CurrStringMem, g_OSPath_r, pathlen);
	memcpy(CurrStringMem + pathlen, ResFile, filelen);
	char* SceneFile = CurrStringMem;

	CurrStringMem += pathlen + filelen;

	memcpy(CurrStringMem, g_OSPath_r, pathlen);
	memcpy(CurrStringMem + pathlen, UIFile, configlen);
	char* GUIFile = CurrStringMem;

	CurrStringMem += pathlen + configlen;

	memcpy(CurrStringMem, g_OSPath_r, pathlen);
	memcpy(CurrStringMem + pathlen, Res2, configlen);
	char* Scene2 = CurrStringMem;

	CurrStringMem += pathlen + configlen;

	// Initialize Core Systems

	if(!glfwInit())
	{
		return EXIT_FAILURE;
	}

	const char* GLSLVersion = "#version 460";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Resize in config, not in render code
	// TODO: use glfwSetWindowAttrib() to allow for one-time resizing operations as menu
	// selections
	glfwWindowHint(GLFW_RESIZABLE, true);

	GLFWwindow* Window = glfwCreateWindow(SCREEN_X_DIM_DEFAULT, SCREEN_Y_DIM_DEFAULT, "void", 0, 0);
	if(!Window)
	{
		printf("GLFW: Failed to create window\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(Window);
	glfwSetFramebufferSizeCallback(Window, FrameResizeCallback);

	// /!\ gladLoadGLLoader() overwrites all gl functions, can only be called after successfully
	// setting a current context

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD: Failed getting function pointers\n");
		return EXIT_FAILURE;
	}

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);
#endif

	window_handler_t* WinHND = InitWindowHandler(SCREEN_X_DIM_DEFAULT, SCREEN_Y_DIM_DEFAULT);
	if(!WinHND)
	{
		printf("System: Could not allocate core window handler\n");
		return EXIT_FAILURE;
	}
	glfwSetWindowUserPointer(Window, (void*)WinHND);

	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(Window, MousePosCallback);

	// Initialize ImGui Context

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	WinHND->ImIO = ImGui::GetIO();
	//(void)WinHND->ImIO;
	WinHND->ImIO.IniFilename = GUIFile;
	ImGui::StyleColorsDark();
	// ImGuiStyle& UIStyle = ImGui::GetStyle();

	bool b_res = false;
	b_res = ImGui_ImplGlfw_InitForOpenGL(Window, true);
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

	// TODO: bounding box construction from min/max params at node level
	// TODO: collision hull at mesh level

	vertex_buffer_info_t VBufferState = {};
	glCreateBuffers(VOID_VBUFCOUNT_FMT, VBufferState.VBufferArray);
	glNamedBufferStorage(VBufferState.VBufferArray[INDEX_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(VBufferState.VBufferArray[POS_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(VBufferState.VBufferArray[NORM_BUFFER],V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(VBufferState.VBufferArray[TAN_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(VBufferState.VBufferArray[TEX_BUFFER], V_MIB(100), 0x0, GL_DYNAMIC_STORAGE_BIT);

	unsigned int VAO;
	glCreateVertexArrays(1, &VAO);

	glVertexArrayElementBuffer(VAO, VBufferState.VBufferArray[INDEX_BUFFER]);	  // sizeof(vec3)
	glVertexArrayVertexBuffer(VAO, 0, VBufferState.VBufferArray[POS_BUFFER], 0, 12);  // sizeof(vec3)
	glVertexArrayVertexBuffer(VAO, 1, VBufferState.VBufferArray[NORM_BUFFER], 0, 12); // sizeof(vec3)
	glVertexArrayVertexBuffer(VAO, 2, VBufferState.VBufferArray[TAN_BUFFER], 0, 16);  // sizeof(vec3)
	glVertexArrayVertexBuffer(VAO, 3, VBufferState.VBufferArray[TEX_BUFFER], 0, 8);	  // sizeof(vec3)

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, 1, 1);

	glEnableVertexArrayAttrib(VAO, 2);
	glVertexArrayAttribFormat(VAO, 2, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, 2, 2);

	glEnableVertexArrayAttrib(VAO, 3);
	glVertexArrayAttribFormat(VAO, 3, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, 3, 3);

	res = LoadSceneFromGLB(SceneFile, WinHND, &VAO, &VBufferState, VOID_TEX_COUNT);
	if(res == EXIT_FAILURE)
	{
		printf("System: Could not load scene file: %s\n", SceneFile);
		return EXIT_FAILURE;
	}
	res = LoadSceneFromGLB(SceneFile, WinHND, &VAO, &VBufferState, VOID_TEX_COUNT);
	if(res == EXIT_FAILURE)
	{
		printf("System: Could not load scene file: %s\n", SceneFile);
		return EXIT_FAILURE;
	}
	res = LoadSceneFromGLB(Scene2, WinHND, &VAO, &VBufferState, VOID_TEX_COUNT);
	if(res == EXIT_FAILURE)
	{
		printf("System: Could not load scene file: %s\n", Scene2);
		return EXIT_FAILURE;
	}

	// TODO: switch to glDrawElementsIndirectCommand
	// /!\ remember to make changes where the draw calls actually happen too

	glBindVertexArray(0);

	// Initialize Render passes

	// Main pass

	res = WinHND->HDRPass.Init(WinHND->Width, WinHND->Height);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main (HDR) pass framebuffer\n");
		return EXIT_FAILURE;
	}

	shader_info_t MainPassParams = {};
	res = MainPassParams.Init("shaders/main.vert", 0, 0, 0, "shaders/main.frag", 0);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main pass shader parameters\n");
		return EXIT_FAILURE;
	}
	res = WinHND->MainShader.Create(MainPassParams);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main pass shaders\n");
		return EXIT_FAILURE;
	}

	model_uni = glGetUniformLocation(WinHND->MainShader.ID, "model");
	minvt_uni = glGetUniformLocation(WinHND->MainShader.ID, "minvt");
	view_uni = glGetUniformLocation(WinHND->MainShader.ID, "view");
	viewpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "viewpos");
	projection_uni = glGetUniformLocation(WinHND->MainShader.ID, "projection");
	lightpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "lightpos");
	objcolor_uni = glGetUniformLocation(WinHND->MainShader.ID, "objcolor");
	lightcolor_uni = glGetUniformLocation(WinHND->MainShader.ID, "lightcolor");
	ambistrgth_uni = glGetUniformLocation(WinHND->MainShader.ID, "ambientstrength");

	// Post pass

	shader_info_t PostPassParams = {};
	res = PostPassParams.Init("shaders/post.vert", 0, 0, 0, "shaders/post.frag", 0);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize post pass shader parameters\n");
		return EXIT_FAILURE;
	}
	res = WinHND->PostShader.Create(PostPassParams);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize post pass shaders\n");
		return EXIT_FAILURE;
	}

	exposure_uni = glGetUniformLocation(WinHND->PostShader.ID, "exposure");

	// Pick pass

	res = WinHND->PickPass.Init(WinHND->Width, WinHND->Height);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize pick pass framebuffer\n");
		return EXIT_FAILURE;
	}

	shader_info_t PickPassParams = {};
	res = PickPassParams.Init("shaders/pick.vert", 0, 0, 0, "shaders/pick.frag", 0);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize pick shader parameters\n");
		return EXIT_FAILURE;
	}
	res = WinHND->PickShader.Create(PickPassParams);
	if(res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize pick pass shaders\n");
		return EXIT_FAILURE;
	}

	unsigned int pickingmodel_uni = glGetUniformLocation(WinHND->PickShader.ID, "model");
	unsigned int pickingview_uni = glGetUniformLocation(WinHND->PickShader.ID, "view");
	unsigned int pickingprojection_uni = glGetUniformLocation(WinHND->PickShader.ID, "projection");
	unsigned int pickingindex_uni = glGetUniformLocation(WinHND->PickShader.ID, "index");
	unsigned int pickingtype_uni = glGetUniformLocation(WinHND->PickShader.ID, "type");

	// Initialize first-frame data

	WinHND->Projection = glm::perspective(glm::radians(VOID_HFOV_DEFAULT),
					      SCREEN_X_DIM_DEFAULT / SCREEN_Y_DIM_DEFAULT, 0.1f, 100.0f);

	glm::mat4 Model = {};
	glm::vec3 LightPosition = {1.2f, 1.0f, 2.0f};
	glm::vec3 lightScale = {0.2f, 0.2f, 0.2f};

	float FrameStartTime = 0;
	float FrameEndTime = 0;
	exposure_val = 0.5;

	glEnable(GL_DEPTH_TEST);
	int RenderMode = GL_TRIANGLES;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Frame loop

	bool HelpWindow = false;
	bool PostWindow = false;
	bool DemoWindow = false;
	while(!glfwWindowShouldClose(Window))
	{
		FrameStartTime = glfwGetTime();

		// Handle user input

		glfwPollEvents();
		WinHND->ImIO = ImGui::GetIO();
		ProcessInput(Window);

		// UI framegen

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		GenerateInterfaceElements(WinHND, &HelpWindow, &PostWindow, &DemoWindow);

		// Render passes

		glBindVertexArray(VAO);

#ifdef DEBUG
		// Mouse Picking Pass for Scene Editor

		WinHND->PickPass.Bind();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		WinHND->PickShader.Use();

		glUniformMatrix4fv(pickingprojection_uni, 1, GL_FALSE, glm::value_ptr(WinHND->Projection));
		glUniformMatrix4fv(pickingview_uni, 1, GL_FALSE, glm::value_ptr(WinHND->View));

		for(unsigned int i = 0; i < WinHND->Scene.NodePosition; i++)
		{
			node_create_info_t Node = WinHND->Scene.Node[i];
			if(Node.Visible == VIS_STATUS_FREED)
			{
				continue;
			}

			glUniform1f(pickingindex_uni, float(i + 1));
			glUniform1f(pickingtype_uni, float(1));

			glUniformMatrix4fv(pickingmodel_uni, 1, GL_FALSE, glm::value_ptr(WinHND->Scene.ModelMatrix[i]));

			mesh_info_t Mesh = WinHND->Scene.Mesh[Node.MeshIndex];
			for(uint32_t t = 0; t < Mesh.size; t++)
			{
				primitive_create_info_t Prim = WinHND->Scene.Prim[Mesh.base_index + t];
				if(Prim.IndexInfo.IndexCount)
				{
					glDrawElementsBaseVertex(RenderMode, Prim.IndexInfo.IndexCount,
								 Prim.IndexInfo.IndexType,
								 (void*)Prim.IndexInfo.ByteOffsetEBO,
								 Prim.VertexInfo.VertexOffset);
				}
				else
				{
					glDrawArrays(RenderMode, Prim.VertexInfo.VertexOffset,
						     Prim.VertexInfo.VAttrCount);
				}
			}
		}
#endif

		// Object Geometry Pass

		WinHND->HDRPass.Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		WinHND->MainShader.Use();

		glUniform1f(ambistrgth_uni, 0.1f);
		glUniform3f(lightcolor_uni, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightpos_uni, LightPosition.x, LightPosition.y, LightPosition.z);

		glUniformMatrix4fv(projection_uni, 1, GL_FALSE, glm::value_ptr(WinHND->Projection));

		glUniformMatrix4fv(view_uni, 1, GL_FALSE, glm::value_ptr(WinHND->View));
		glUniform3f(viewpos_uni, WinHND->Camera.Position.x, WinHND->Camera.Position.y,
			    WinHND->Camera.Position.z);

		for(unsigned int i = 0; i < WinHND->Scene.NodePosition; i++)
		{
			node_create_info_t Node = WinHND->Scene.Node[i];
			if(Node.Visible == VIS_STATUS_FREED)
			{
				continue;
			}
			float butt = sinf(glfwGetTime());
			WinHND->Scene.ModelMatrix[i][3][0] += butt;
			WinHND->Scene.ModelMatrix[i][3][1] += butt;

			glUniformMatrix4fv(model_uni, 1, GL_FALSE, glm::value_ptr(WinHND->Scene.ModelMatrix[i]));

			mesh_info_t Mesh = WinHND->Scene.Mesh[Node.MeshIndex];
			for(uint32_t t = 0; t < Mesh.size; t++)
			{
				primitive_create_info_t Prim = WinHND->Scene.Prim[Mesh.base_index + t];

				glUniformMatrix3fv(minvt_uni, 1, GL_FALSE, glm::value_ptr(Prim.ModelInvTrans));
				glUniform3fv(objcolor_uni, 1, glm::value_ptr(Prim.Color));

				glBindTextureUnit(0, Prim.TexInfo.TexArray[0]);
				glBindTextureUnit(1, Prim.TexInfo.TexArray[1]);
				glBindTextureUnit(2, Prim.TexInfo.TexArray[2]);

				if(Prim.IndexInfo.IndexCount)
				{
					glDrawElementsBaseVertex(RenderMode, Prim.IndexInfo.IndexCount,
								 Prim.IndexInfo.IndexType,
								 (void*)Prim.IndexInfo.ByteOffsetEBO,
								 Prim.VertexInfo.VertexOffset);
				}
				else
				{
					glDrawArrays(RenderMode, Prim.VertexInfo.VertexOffset,
						     Prim.VertexInfo.VAttrCount);
				}
			}
		}

		if(WinHND->ActiveSelection)
		{
			/*
			WinHND->Active.ComposeModelM4();
			glUniformMatrix4fv(model_uni, 1, GL_FALSE, &WinHND->Active.Model.m[0][0]);
			glUniform3fv(objcolor_uni, 1, &WinHND->Active.Color.x);
			glDrawElements(RenderMode, indexcount, indextype, (void*)0);
			*/
		}

		// Light Geometry Pass

		glUniform1f(ambistrgth_uni, 1.0f);
		glUniform3f(objcolor_uni, 1.0f, 1.0f, 1.0f);
		Model = glm::mat4(1.0f);
		Model = glm::scale(Model, lightScale);
		Model = glm::translate(Model, LightPosition);
		glUniformMatrix4fv(model_uni, 1, GL_FALSE, glm::value_ptr(Model));

		/*
		if(WinHND->GeometryObjects.IndexCount[0])
		{
			glDrawElementsBaseVertex(RenderMode, WinHND->GeometryObjects.IndexCount[0],
			    WinHND->GeometryObjects.IndexType[0],
			    (void*)WinHND->GeometryObjects.ByteOffsetEBO[0],
			    WinHND->GeometryObjects.OffsetVBO[0]);
		}
		else
		{
			glDrawArrays(RenderMode, WinHND->GeometryObjects.OffsetVBO[0],
		WinHND->GeometryObjects.VAttrCount[0]);
		}
		*/

		// Blit from HDR to linear normal quad for post-processing, parse inter-frame data

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, WinHND->HDRPass.ColorBuffer);

		WinHND->PostShader.Use();
		glUniform1f(exposure_uni, float(exposure_val));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Attributes hard coded in vertex shader

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		FrameEndTime = glfwGetTime();
		WinHND->FrameTimeMS = (FrameEndTime - FrameStartTime) * 1000.0f;

		glfwSwapBuffers(Window);

		WinHND->DeltaTime = FrameEndTime - WinHND->PrevFrameTime;
		WinHND->PrevFrameTime = FrameEndTime;
	}

	// Free resources and exit - not technically necessary when this is the end of the program,
	// but future-proofs for mutlithreading or other integrations

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	free(WinHND);
	WinHND = 0x0;

	return EXIT_SUCCESS;
}

void FrameResizeCallback(GLFWwindow* Window, int new_screen_width, int new_screen_height)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	WinHND->Width = new_screen_width;
	WinHND->Height = new_screen_height;

	// Also resize camera frustum and all existing framebuffers
	WinHND->Projection = glm::perspective(glm::radians(VOID_HFOV_DEFAULT),
					      (float)new_screen_width / (float)new_screen_height, 0.1f, 100.0f);
	WinHND->HDRPass.Release();
	WinHND->HDRPass.Init(new_screen_width, new_screen_height);
	WinHND->PickPass.Release();
	WinHND->PickPass.Init(new_screen_width, new_screen_height);

	glViewport(0, 0, new_screen_width, new_screen_height);
}

void ProcessInput(GLFWwindow* Window)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	// Check if the UI should be pulling focus
	if(WinHND->ImIO.WantCaptureKeyboard)
	{
		return;
	}

	WinHND->Camera.Speed = 2.5f * WinHND->DeltaTime;
	WinHND->Camera.RelativeXAxis = glm::normalize(glm::cross(WinHND->Camera.Eye, WinHND->Camera.UpAxis));
	WinHND->Camera.RelativeYAxis = glm::normalize(glm::cross(WinHND->Camera.RelativeXAxis, WinHND->Camera.Eye));
	WinHND->Camera.Move(Window);
	WinHND->View = glm::lookAt(WinHND->Camera.Position, WinHND->Camera.Position + WinHND->Camera.Eye,
				   WinHND->Camera.UpAxis);

	if(glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS || WinHND->ShouldExit)
	{
		glfwSetWindowShouldClose(Window, true);
	}
	if(glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if(glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if(glfwGetKey(Window, GLFW_KEY_P) == GLFW_PRESS)
	{
		PKeyWasDown = 1;
	}
	if(glfwGetKey(Window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		if(PKeyWasDown || WinHND->ReloadShaders)
		{
			WinHND->MainShader.Rebuild();
			model_uni = glGetUniformLocation(WinHND->MainShader.ID, "model");
			minvt_uni = glGetUniformLocation(WinHND->MainShader.ID, "minvt");
			view_uni = glGetUniformLocation(WinHND->MainShader.ID, "view");
			viewpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "viewpos");
			projection_uni = glGetUniformLocation(WinHND->MainShader.ID, "projection");
			lightpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "lightpos");
			objcolor_uni = glGetUniformLocation(WinHND->MainShader.ID, "objcolor");
			lightcolor_uni = glGetUniformLocation(WinHND->MainShader.ID, "lightcolor");
			ambistrgth_uni = glGetUniformLocation(WinHND->MainShader.ID, "ambientstrength");

			PKeyWasDown = 0;
			WinHND->ReloadShaders = false;
		}
	}
	if(glfwGetKey(Window, GLFW_KEY_N) == GLFW_PRESS)
	{
		NKeyWasDown = 1;
	}
	if(glfwGetKey(Window, GLFW_KEY_N) == GLFW_RELEASE)
	{
		if(NKeyWasDown)
		{
			/*
			uMATH::vec3f_t p = { 0.0f,0.0f,4.5f };
			WinHND->Active.Model = WinHND->View;
			uMATH::Translate(&WinHND->Active.Model, p);
			WinHND->Active.Model = uMATH::InverseM4(WinHND->Active.Model);
			WinHND->Active.DecomposeModelM4();
			WinHND->Active.New = false;
			WinHND->ActiveSelection = true;
			*/
		}
		NKeyWasDown = 0;
	}

	// Have to separately check if the UI should be pulling mouse button inputs, as they aren't
	// tracked by WantCaptureKeyboard
	if(WinHND->ImIO.WantCaptureMouse)
	{
		return;
	}

	if(glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		LMouseWasDown = 1;
	}
	if(glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		if(LMouseWasDown)
		{
			/*
			texel_info_t res = WinHND->PickPass.GetInfo((uint32_t)WinHND->PrevMouseX,
								    (uint32_t)(WinHND->Height - WinHND->PrevMouseY));
			if(WinHND->ActiveSelection && WinHND->Active.Deleted != true)
			{
				WinHND->Active.ComposeModelM4();
				WinHND->GeometryObjects.Alloc(WinHND->Active);
				WinHND->ActiveSelection = false;
			}
			if (res.ID > 0)
			{
				WinHND->Active.Model = WinHND->GeometryObjects.Model[(int)res.ID -
			1]; WinHND->Active.Color = WinHND->GeometryObjects.Color[(int)res.ID - 1];
				WinHND->Active.DecomposeModelM4();
				WinHND->GeometryObjects.Free((int)res.ID - 1);
				WinHND->ActiveSelection = true;
			}
			*/
		}
		LMouseWasDown = 0;
	}
	if(glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if(!RMouseWasDown)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		RMouseWasDown = 1;
	}
	if(glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		if(RMouseWasDown)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		RMouseWasDown = 0;
	}
}

void MousePosCallback(GLFWwindow* Window, double mx, double my)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	// Check if the UI should be pulling focus
	if(WinHND->ImIO.WantCaptureMouse)
	{
		return;
	}

	if(WinHND->FirstCameraMove)
	{
		WinHND->PrevMouseX = mx;
		WinHND->PrevMouseY = my;
		WinHND->FirstCameraMove = 0;
	}

	float xoffset = mx - WinHND->PrevMouseX;
	float yoffset = WinHND->PrevMouseY - my;

	WinHND->PrevMouseX = mx;
	WinHND->PrevMouseY = my;

	if(RMouseWasDown)
	{
		WinHND->Camera.LookAtMouse(xoffset, yoffset);
	}
}

void GenerateInterfaceElements(window_handler_t* WinHND, bool* HelpWindow, bool* PostWindow, bool* DemoWindow)
{
	if(WinHND->ActiveSelection)
	{
		ImGui::Begin("Object Parameters");

		/*
		ImGui::Spacing();
		ImGui::SliderFloat("Scale", &WinHND->Active.Scale, 0.1f, 2.5f);
		ImGui::Spacing();
		ImGui::Text("Rotation");
		ImGui::SliderFloat("Angle", &WinHND->Active.RotationAngle, 0.0f, 180.0f);
		ImGui::SliderFloat("rX", &WinHND->Active.RotationAxis.x, 0.0f, 1.0f);
		ImGui::SliderFloat("rY", &WinHND->Active.RotationAxis.y, 0.0f, 1.0f);
		ImGui::SliderFloat("rZ", &WinHND->Active.RotationAxis.z, 0.0f, 1.0f);
		ImGui::Spacing();
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &WinHND->Active.Position.x, -15.0f, 15.0f);
		ImGui::SliderFloat("Y", &WinHND->Active.Position.y, -15.0f, 15.0f);
		ImGui::SliderFloat("Z", &WinHND->Active.Position.z, -15.0f, 15.0f);
		ImGui::Spacing();
		ImGui::ColorEdit3("Color", (float*)&WinHND->Active.Color);
		ImGui::Spacing();
		if (ImGui::Button("Delete Object"))
		{
			WinHND->Active.Deleted = true;
			WinHND->ActiveSelection = false;
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
		WinHND->ReloadShaders = true;
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
		WinHND->ShouldExit = true;
	}
	ImGui::Spacing();
	ImGui::Text("CPU Frame time: %.2f ms", WinHND->FrameTimeMS);
	ImGui::Spacing();
	ImGui::Text("Frame present time: %.2f ms", WinHND->DeltaTime * 1000.0f);

	ImGui::End();
}

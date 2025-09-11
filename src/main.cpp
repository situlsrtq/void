#include "main.h"
#include "u_math.h"


// TODO: Get rid of runtime path discovery in release builds


#ifdef DEBUG
void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
	 (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
	 type, severity, message);
}
#endif


uint8_t NKeyWasDown;
uint8_t RKeyWasDown;
uint8_t PKeyWasDown;  
uint8_t LMouseWasDown;
uint8_t RMouseWasDown;

unsigned int model_uni;
unsigned int view_uni;
unsigned int viewpos_uni;
unsigned int projection_uni;
unsigned int lightpos_uni;
unsigned int objcolor_uni;
unsigned int lightcolor_uni;
unsigned int ambistrgth_uni;
unsigned int exposure_uni;
float exposure_val;


cgltf_attribute* FindAttrType(const cgltf_primitive& prim, cgltf_attribute_type type)
{
	cgltf_attribute* attr = &prim.attributes[0];

	for(int i = 0; i < prim.attributes_count; i++)
	{
		if(prim.attributes[i].type != type) continue;

		attr = &prim.attributes[i];
		return attr;
	}

	attr = 0x0;
	printf("GLTF: Attribute type not found %s\n", "peepee, map to attr_type later");
	return attr;
}


int main(void)
{
	int res = PAL::GetPath(g_PathBuffer_r, VOID_PATH_MAX)				;
	if(res != EXIT_SUCCESS)
	{
		printf("PAL: Failed to initialize file path\n");
	}

	// TODO: per-thread string memory system, to be sized based on thread's need. Rendering thread will be heaviest user

	char* ResourceStringMem = (char *)malloc(4 * V_MIB);
	size_t len = strlen(g_OSPath_r);
	memcpy(ResourceStringMem, g_OSPath_r, len);
	memcpy(ResourceStringMem + len, "res/DamagedHelmet.glb", 22);
	char* SceneFile = ResourceStringMem;

	// Initialize Core Systems

	if (!glfwInit())
	{
		 return EXIT_FAILURE;
	}

	const char* GLSLVersion = "#version 460";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * Window = glfwCreateWindow(SCREEN_X_DIM_DEFAULT,SCREEN_Y_DIM_DEFAULT,"mBox",0,0);
	if(!Window)
	{	
		printf("GLFW: Failed to create window\n");
		glfwTerminate();
		 return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(Window);
	glfwSetFramebufferSizeCallback(Window, FrameResizeCallback);

	// /!\ gladLoadGLLoader() overwrites all gl functions, can only be called after successfully setting a current context 

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD: Failed getting function pointers\n");
		 return EXIT_FAILURE;
	}

	window_handler_t* WinHND = InitWindowHandler(SCREEN_X_DIM_DEFAULT, SCREEN_Y_DIM_DEFAULT);
	if (!WinHND)
	{
		printf("System: Could not allocate core window handler\n");
		 return EXIT_FAILURE;
	}
	glfwSetWindowUserPointer(Window, (void *)WinHND);

	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(Window, MousePosCallback);

	// Initialize ImGui Context

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	WinHND->ImIO = ImGui::GetIO(); (void)WinHND->ImIO;
	ImGui::StyleColorsDark();

	ImGuiStyle& UIStyle = ImGui::GetStyle();

	bool b_res = false;
	b_res = ImGui_ImplGlfw_InitForOpenGL(Window, true);
	if (!b_res)
	{
		printf("System: Could not initialize imgui context for GLFW\n");
		 return EXIT_FAILURE;
	}
	b_res = ImGui_ImplOpenGL3_Init(GLSLVersion);
	if (!b_res)
	{
		printf("System: Could not initialize imgui context for OpenGL\n");
		 return EXIT_FAILURE;
	}

	// TODO: turn gltf processing into a module, define a standardized gltf format for game resources and support only that. 
	// ideally, we call cgltf_free() immediately after doing this processing

	// Initialize mesh data and positions
	void* buf;
	size_t size;

	cgltf_options opt;
	memset(&opt, 0, sizeof(opt));
	cgltf_data* data = 0x0;

	cgltf_result cgl_res = cgltf_parse_file(&opt, SceneFile, &data);

	if(cgl_res == cgltf_result_success) cgl_res = cgltf_load_buffers(&opt, data, SceneFile);
	if(cgl_res == cgltf_result_success) cgl_res = cgltf_validate(data);

	uint8_t* DataBaseAddr = (uint8_t *)data->buffers->data;
	cgltf_primitive& prim = data->meshes[0].primitives[0];

	GLint indextype; 
	if(prim.indices->component_type == cgltf_component_type_r_16u) indextype = GL_UNSIGNED_SHORT;
	else indextype = GL_UNSIGNED_INT;

	size_t indexcount = prim.indices->count;
	cgltf_attribute* attr;
	size_t meshsize = 0;
	uint8_t* CombinedData = (uint8_t *)malloc(10 * V_MIB);
	
	attr = FindAttrType(prim, cgltf_attribute_type_position);
	memcpy(CombinedData + meshsize, (DataBaseAddr + attr->data->buffer_view->offset), attr->data->buffer_view->size);
	meshsize += attr->data->buffer_view->size;

	attr = FindAttrType(prim, cgltf_attribute_type_normal);
	memcpy(CombinedData + meshsize, (DataBaseAddr + attr->data->buffer_view->offset), attr->data->buffer_view->size);
	meshsize += attr->data->buffer_view->size;

	attr = FindAttrType(prim, cgltf_attribute_type_texcoord);
	memcpy(CombinedData + meshsize, (DataBaseAddr + attr->data->buffer_view->offset), attr->data->buffer_view->size);
	meshsize += attr->data->buffer_view->size;

	// Initialize Core VBO, VAO, Render passes

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);
#endif
	
	// TODO: Switch to one buffer per level and using glBufferSubData/glDrawElementsBaseVertex - eventually glDrawElementsIndirectCommand
	// /!\ remember to make changes where the draw calls actually happen too

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, prim.indices->buffer_view->size, (void *)(DataBaseAddr + prim.indices->buffer_view->offset), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, meshsize, (void *)CombinedData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(prim.attributes[0].data->buffer_view->size));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)(prim.attributes[0].data->buffer_view->size + prim.attributes[1].data->buffer_view->size));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	cgltf_free(data);

	//Main pass
	
	res = WinHND->HDRPass.Init(WinHND->Width, WinHND->Height);
	if (res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main (HDR) pass framebuffer\n");
		 return EXIT_FAILURE;
	}

	shader_info_t MainPassParams = {};
	res = MainPassParams.Init("shaders/main.vert",0,0,0,"shaders/main.frag",0);
	if (res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main pass shader parameters\n");
		return EXIT_FAILURE;
	}
	res = WinHND->MainShader.Create(MainPassParams);
	if (res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize main pass shaders\n");
		return EXIT_FAILURE;
	}

	model_uni = glGetUniformLocation(WinHND->MainShader.ID, "model");
	view_uni = glGetUniformLocation(WinHND->MainShader.ID, "view");
	viewpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "viewpos");
	projection_uni = glGetUniformLocation(WinHND->MainShader.ID, "projection");
	lightpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "lightpos");
	objcolor_uni = glGetUniformLocation(WinHND->MainShader.ID, "objcolor");
	lightcolor_uni = glGetUniformLocation(WinHND->MainShader.ID, "lightcolor");
	ambistrgth_uni = glGetUniformLocation(WinHND->MainShader.ID, "ambientstrength");

	//Post pass
	
	shader_info_t PostPassParams = {};
	res = PostPassParams.Init("shaders/post.vert",0,0,0,"shaders/post.frag",0);
	if (res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize post pass shader parameters\n");
		return EXIT_FAILURE;
	}
	res = WinHND->PostShader.Create(PostPassParams);
	if (res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize post pass shaders\n");
		return EXIT_FAILURE;
	}

	exposure_uni  = glGetUniformLocation(WinHND->PostShader.ID, "exposure");

	//Pick pass
	
	res = WinHND->PickPass.Init(WinHND->Width, WinHND->Height);
	if (res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize pick pass framebuffer\n");
		 return EXIT_FAILURE;
	}

	shader_info_t PickPassParams = {};
	res = PickPassParams.Init("shaders/pick.vert",0,0,0,"shaders/pick.frag",0);
	if (res != EXIT_SUCCESS)
	{
		printf("System: Failed to initialize pick shader parameters\n");
		return EXIT_FAILURE;
	}
	res = WinHND->PickShader.Create(PickPassParams);
	if (res != EXIT_SUCCESS)
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
	uMATH::vec3f_t position = {};

	uMATH::mat4f_t GeometryModel = {};
	geometry_create_info_t CreateInfo;
	CreateInfo.Scale = 1.0f;
	CreateInfo.Intensity = 0.5f;
	CreateInfo.Color = { 1.0f, 0.5f, 0.31f };
	float angle = 90.0f;
	uMATH::vec3f_t rVec = {1.0f, 0.0f, 0.0001f};

	SetTransform(&GeometryModel);
	uMATH::Scale(&GeometryModel, CreateInfo.Scale);
	uMATH::MatrixRotate(&GeometryModel, angle, rVec);
	uMATH::Translate(&GeometryModel, position);

	CreateInfo.Model = GeometryModel;
	WinHND->GeometryObjects.Alloc(CreateInfo);

	uMATH::SetFrustumHFOV(&WinHND->Projection, 45.0f, SCREEN_X_DIM_DEFAULT / SCREEN_Y_DIM_DEFAULT, 0.1f, 100.0f);

	uMATH::mat4f_t Model = {};
	uMATH::vec3f_t LightPosition = { 1.2f, 1.0f, 2.0f };
	float lightScale = 0.2f;

	float CurrFrameTime = 0;
	exposure_val = 0.5;

	glEnable(GL_DEPTH_TEST);
	int RenderMode = GL_TRIANGLES;
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	// Frame loop

	bool HelpWindow = false;
	bool PostWindow = false;
	bool DemoWindow = false;
	while (!glfwWindowShouldClose(Window))
	{
		// Handle user input

		glfwPollEvents();
		WinHND->ImIO = ImGui::GetIO();
		ProcessInput(Window);

		// UI framegen

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		GenerateInterfaceElements(WinHND, &HelpWindow, &PostWindow, &DemoWindow);

		//Render passes

		// Mouse Picking Pass

		glBindVertexArray(VAO);

		WinHND->PickPass.Bind_W();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		WinHND->PickShader.Use();

		glUniformMatrix4fv(pickingprojection_uni, 1, GL_FALSE, &WinHND->Projection.m[0][0]);
		glUniformMatrix4fv(pickingview_uni, 1, GL_FALSE, &WinHND->View.m[0][0]);

		for (unsigned int i = 0; i < WinHND->GeometryObjects.Position; i++)
		{
			if (WinHND->GeometryObjects.Visible[i] == VIS_STATUS_FREED)
			{
				continue;
			}

			glUniform1f(pickingindex_uni, float(i + 1));
			glUniform1f(pickingtype_uni, float(1));

			glUniformMatrix4fv(pickingmodel_uni, 1, GL_FALSE, &WinHND->GeometryObjects.Model[i].m[0][0]);
			glDrawElements(RenderMode, indexcount, indextype, (void*)0);
		}

		WinHND->PickPass.Unbind_W();

		// Object Geometry Pass
		
		WinHND->HDRPass.Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		WinHND->MainShader.Use();

		glUniform1f(ambistrgth_uni, 0.1f);
		glUniform3f(lightcolor_uni, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightpos_uni, LightPosition.x, LightPosition.y, LightPosition.z);

		glUniformMatrix4fv(projection_uni, 1, GL_FALSE, &WinHND->Projection.m[0][0]);

		glUniformMatrix4fv(view_uni, 1, GL_FALSE, &WinHND->View.m[0][0]);
		glUniform3f(viewpos_uni, WinHND->Camera.Position.x, WinHND->Camera.Position.y, WinHND->Camera.Position.z);

		for (unsigned int i = 0; i < WinHND->GeometryObjects.Position; i++)
		{
			if (WinHND->GeometryObjects.Visible[i] == VIS_STATUS_FREED)
			{
				continue;
			}

			glUniformMatrix4fv(model_uni, 1, GL_FALSE, &WinHND->GeometryObjects.Model[i].m[0][0]);
			glUniform3fv(objcolor_uni, 1, &WinHND->GeometryObjects.Color[i].x);
			glDrawElements(RenderMode, indexcount, indextype, (void*)0);
		}

		if (WinHND->ActiveSelection)
		{
			WinHND->Active.ComposeModelM4();
			glUniformMatrix4fv(model_uni, 1, GL_FALSE, &WinHND->Active.Model.m[0][0]);
			glUniform3fv(objcolor_uni, 1, &WinHND->Active.Color.x);
			glDrawElements(RenderMode, indexcount, indextype, (void*)0);
		}

		// Light Geometry Pass

		glUniform1f(ambistrgth_uni, 1.0f);
		glUniform3f(objcolor_uni, 1.0f, 1.0f, 1.0f);
		SetTransform(&Model);
		uMATH::Scale(&Model, lightScale);
		uMATH::Translate(&Model, LightPosition);
		glUniformMatrix4fv(model_uni, 1, GL_FALSE, &Model.m[0][0]);
		glDrawElements(RenderMode, indexcount, indextype, (void*)0);

		// Blit from HDR to linear normal quad for post-processing, parse inter-frame data

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, WinHND->HDRPass.ColorBuffer);

		WinHND->PostShader.Use();
		glUniform1f(exposure_uni, float(exposure_val));
		// Positions are currently stored in the vertex shader, not VAO
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Window);

		glBindVertexArray(0);
		glUseProgram(0);

		CurrFrameTime = glfwGetTime();
		WinHND->DeltaTime = CurrFrameTime - WinHND->PrevFrameTime;
		WinHND->PrevFrameTime = CurrFrameTime;
	}

	// Free resources and exit - not technically necessary when this is the end of the program, but future-proofs for mutlithreading or other integrations

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	free(WinHND);
	WinHND = 0x0;

	return EXIT_SUCCESS;
}


void FrameResizeCallback(GLFWwindow *Window, int width, int height)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	WinHND->Width = width;
	WinHND->Height = height;

	// Also resize camera frustum and attached framebuffers
	// TODO: Calculate new FOV instead of using fixed 45 degrees
	uMATH::SetTransform(&WinHND->Projection);
	uMATH::SetFrustumHFOV(&WinHND->Projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	WinHND->HDRPass.Release();
	WinHND->HDRPass.Init(width, height);
	WinHND->PickPass.Release();
	WinHND->PickPass.Init(width, height);

	glViewport(0,0,width,height);
}


void ProcessInput(GLFWwindow *Window)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	// Check if the UI should be pulling focus
	if (WinHND->ImIO.WantCaptureKeyboard)
	{
		return;
	}

	WinHND->Camera.Speed = 2.5f * WinHND->DeltaTime;
	WinHND->Camera.RelativeXAxis = uMATH::Normalize(uMATH::Cross(WinHND->Camera.Eye, WinHND->Camera.UpAxis));
	WinHND->Camera.RelativeYAxis = uMATH::Normalize(uMATH::Cross(WinHND->Camera.RelativeXAxis, WinHND->Camera.Eye));
	WinHND->Camera.Move(Window);
	uMATH::SetCameraView(&WinHND->View, WinHND->Camera.Position, WinHND->Camera.Position + WinHND->Camera.Eye, WinHND->Camera.UpAxis);

	if(glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS || WinHND->ShouldExit)
	{
		glfwSetWindowShouldClose(Window, true);
	}
	if(glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}
	if(glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}
	if (glfwGetKey(Window, GLFW_KEY_P) == GLFW_PRESS)
	{
		PKeyWasDown = 1;
	}
	if(glfwGetKey(Window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		if (PKeyWasDown || WinHND->ReloadShaders)
		{
			WinHND->MainShader.Rebuild();
			model_uni = glGetUniformLocation(WinHND->MainShader.ID, "model");
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
	if (glfwGetKey(Window, GLFW_KEY_N) == GLFW_PRESS)
	{
		NKeyWasDown = 1;
	}
	if (glfwGetKey(Window, GLFW_KEY_N) == GLFW_RELEASE)
	{
		if (NKeyWasDown || WinHND->Active.New == true)
		{
			uMATH::vec3f_t p = { 0.0f,0.0f,4.5f };
			WinHND->Active.Model = WinHND->View;
			uMATH::Translate(&WinHND->Active.Model, p);
			WinHND->Active.Model = uMATH::InverseM4(WinHND->Active.Model);
			WinHND->Active.DecomposeModelM4();
			WinHND->Active.New = false;
			WinHND->ActiveSelection = true;
		}
		NKeyWasDown = 0;
	}

	// Have to separately check if the UI should be pulling mouse button inputs, as they aren't tracked by WantCaptureKeyboard
	if (WinHND->ImIO.WantCaptureMouse)
	{
		return;
	}

	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		LMouseWasDown = 1;
	}
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		if(LMouseWasDown)
		{
			texel_info_t res = WinHND->PickPass.GetInfo((uint32_t)WinHND->PrevMouseX, (uint32_t)(WinHND->Height - WinHND->PrevMouseY));
			if (WinHND->ActiveSelection && WinHND->Active.Deleted != true)
			{
				WinHND->Active.ComposeModelM4();
				WinHND->GeometryObjects.Alloc(WinHND->Active);
				WinHND->ActiveSelection = false;
			}
			if (res.ID > 0)
			{
				WinHND->Active.Model = WinHND->GeometryObjects.Model[(int)res.ID - 1];
				WinHND->Active.Color = WinHND->GeometryObjects.Color[(int)res.ID - 1];
				WinHND->Active.DecomposeModelM4();
				WinHND->GeometryObjects.Free((int)res.ID - 1);
				WinHND->ActiveSelection = true;
			}
		}
		LMouseWasDown = 0;
		WinHND->Active.Deleted = false;
	}
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (!RMouseWasDown)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		RMouseWasDown = 1;
	}
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		if (RMouseWasDown)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		RMouseWasDown = 0;
	}
}


void MousePosCallback(GLFWwindow *Window, double mx, double my)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	// Check if the UI should be pulling focus
	if (WinHND->ImIO.WantCaptureMouse)
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

	if (RMouseWasDown)
	{
		WinHND->Camera.LookAtMouse(xoffset, yoffset);
	}
}


void GenerateInterfaceElements(window_handler_t *WinHND, bool *HelpWindow, bool *PostWindow, bool *DemoWindow)
{
	if(WinHND->ActiveSelection)
	{
		ImGui::Begin("Object Parameters");

		ImGui::Text("");
		ImGui::SliderFloat("Scale", &WinHND->Active.Scale, 0.1f, 2.5f);
		ImGui::Text("");
		ImGui::Text("Rotation");
		ImGui::SliderFloat("Angle", &WinHND->Active.RotationAngle, 0.0f, 180.0f);
		ImGui::SliderFloat("rX", &WinHND->Active.RotationAxis.x, 0.0f, 1.0f);
		ImGui::SliderFloat("rY", &WinHND->Active.RotationAxis.y, 0.0f, 1.0f);
		ImGui::SliderFloat("rZ", &WinHND->Active.RotationAxis.z, 0.0f, 1.0f);
		ImGui::Text("");
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &WinHND->Active.Position.x, -15.0f, 15.0f);
		ImGui::SliderFloat("Y", &WinHND->Active.Position.y, -15.0f, 15.0f);
		ImGui::SliderFloat("Z", &WinHND->Active.Position.z, -15.0f, 15.0f);
		ImGui::Text("");
		ImGui::ColorEdit3("Color", (float*)&WinHND->Active.Color);
		ImGui::Text("");
		if (ImGui::Button("Delete Object"))
		{
			WinHND->Active.Deleted = true;
			WinHND->ActiveSelection = false;
		}

		ImGui::End();
	}
	if(*PostWindow)
	{
		ImGui::Begin("Post-Processing");

		ImGui::Text("");
		ImGui::SliderFloat("Exposure", &exposure_val, 0.0f, 5.0f);
		ImGui::Text("");
		if (ImGui::Button("Close"))
		{
			*PostWindow = false;
		}

		ImGui::End();
	}
	if(*HelpWindow)
	{
		ImGui::Begin("Help");

		ImGui::Text("Use WASD keys to move left, right, forward, and backward");
		ImGui::Text("");
		ImGui::Text("Use LShift to move down. Use Spacebar to move up");
		ImGui::Text("");
		ImGui::Text("Hold Right Mouse button to control camera with mouse movement");
		ImGui::Text("");
		ImGui::Text("UI elements capture mouse commands when hovered.");
		ImGui::Text("Move the mouse outside the menu to interact with the scene");
		ImGui::Text("");
		if (ImGui::Button("Close"))
		{
			*HelpWindow = false;
		}

		ImGui::End();
	}
/*
#ifdef DEBUG
	else if (*DemoWindow)
	{
		ImGui::ShowDemoWindow(DemoWindow);
	}
#endif
*/
	ImGui::Begin("Scene Controls");

	ImGui::Text("");
	if (ImGui::Button("New Object (n)"))
	{
		WinHND->Active.New = true;
	}
	ImGui::Text("");
	if (ImGui::Button("Post-Processing"))
	{
		*PostWindow = true;
	}
	ImGui::Text("");
	if (ImGui::Button("Reload Shaders (p)"))
	{
		WinHND->ReloadShaders = true;
	}
	ImGui::Text("");
	if (ImGui::Button("Help"))
	{
		*HelpWindow = true;
	}
	/*
#ifdef DEBUG
		ImGui::SameLine();
		if (ImGui::Button("Show Demo Window"))
		{
			*DemoWindow = true;
		}
#endif
*/
	ImGui::Text("");
	if (ImGui::Button("Exit (esc)"))
	{
		WinHND->ShouldExit = true;
	}
	ImGui::Text("");
	ImGui::Text("Frame time: %.4f ms", WinHND->DeltaTime);

	ImGui::End();
}

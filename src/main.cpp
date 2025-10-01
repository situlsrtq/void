#include "main.h"


// TODO: Get rid of runtime path discovery in release builds


#ifdef DEBUG
void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	(void)source;
	(void)userParam;
	(void)length;
	(void)id;

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


cgltf_attribute* FindAttrType(const cgltf_primitive* prim, cgltf_attribute_type type)
{
	cgltf_attribute* attr = &prim->attributes[0];

	for(uint32_t i = 0; i < prim->attributes_count; i++)
	{
		if(prim->attributes[i].type != type) continue;

		attr = &prim->attributes[i];
		return attr;
	}

	attr = 0x0;
	printf("GLTF: Attribute type not found %s\n", "peepee, map to attr_type later");
	return attr;
}

void GetNodeMatrix(glm::mat4* m, cgltf_node* node)
{
	glm::vec3 trv = {node->translation[0],node->translation[1],node->translation[2]};
	glm::vec3 scl = {node->scale[0],node->scale[1],node->scale[2]};
	glm::quat p = {node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]};
	glm::mat4 r = glm::mat4_cast(p);

	*m = glm::mat4(1.0f);
	*m = glm::translate(*m, trv);
	*m *= r;
	*m = glm::scale(*m, scl);	
}


int main(void)
{
	int res = PAL::GetPath(g_PathBuffer_r, VOID_PATH_MAX);
	if(res != EXIT_SUCCESS)
	{
		printf("PAL: Failed to initialize file path\n");
	}

	// TODO: per-thread string memory system, to be sized based on thread's need. Rendering thread will be heaviest user

	void* ResourceStringMem = (char *)malloc(4 * V_MIB);
	char* CurrStringMem = (char *)ResourceStringMem;
	const char* ResFile = "res/sponza.glb";
	const char* UIFile = "config/imgui.ini";
	// Drop the null terminator on OSPath intentionally, since it will be concatenated with paths.
	// hacky stupid shit, will not last
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
	
	// Initialize Core Systems

	if (!glfwInit())
	{
		 return EXIT_FAILURE;
	}

	const char* GLSLVersion = "#version 460";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Resize in config, not in render code
	// TODO: use glfwSetWindowAttrib() to allow for one-time resizing operations as menu selections
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* Window = glfwCreateWindow(SCREEN_X_DIM_DEFAULT,SCREEN_Y_DIM_DEFAULT,"void",0,0);
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

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);
#endif

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
	WinHND->ImIO.IniFilename = GUIFile;
	ImGui::StyleColorsDark();
	//ImGuiStyle& UIStyle = ImGui::GetStyle();

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

//------------------------------------------------------------------------------------------------------------
	
	// TODO: turn gltf processing into a module, define a standardized gltf format for game resources and support only that. 
	// TODO: pass in VAO/EBO?VBO?
	// TODO: texture loading
	// TODO: bounding box construction from min/max params at node level
	// TODO: collision hull at mesh level

	// Initialize mesh data and positions

	size_t NumSceneBytes;
	res = UTIL::GetFileSize(SceneFile, &NumSceneBytes);
	if(res == EXIT_FAILURE)
	{
		printf("Resources: could not access scene file %s\n", SceneFile);
		return EXIT_FAILURE;
	}

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumSceneBytes, 0x0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, NumSceneBytes, 0x0, GL_DYNAMIC_DRAW);

	cgltf_options opt;
	memset(&opt, 0, sizeof(opt));
	cgltf_data* data = 0x0;

	cgltf_result cgl_res = cgltf_parse_file(&opt, SceneFile, &data);
	if(cgl_res != cgltf_result_success) 
	{
		printf("GLTF Load: could not parse file %s", SceneFile);
		return EXIT_FAILURE;
	}

	cgl_res = cgltf_load_buffers(&opt, data, SceneFile);
	if(cgl_res != cgltf_result_success) 
	{
		printf("GLTF Load: could not validate file %s", SceneFile);
		return EXIT_FAILURE;
	}

	cgl_res = cgltf_validate(data);
	if(cgl_res != cgltf_result_success) 
	{
		printf("GLTF Load: could not validate file %s", SceneFile);
		return EXIT_FAILURE;
	}

	uint8_t* DataBaseAddr = (uint8_t *)data->buffers->data;
	uint64_t OffsetEBO = 0;
	uint64_t OffsetVBO = 0;
	uint8_t EBOPadding = 0;
	geometry_create_info_t CreateInfo;

	for(uint32_t i = 0; i < data->nodes_count; i++)
	{
		cgltf_node* node = &data->nodes[i];

		uint64_t absbufferoffset;
		uint64_t relbufferoffset;
		uint64_t count;
		uint8_t stride;
		glm::mat4 nodematrix;
		GetNodeMatrix(&nodematrix, node);

		cgltf_mesh* mesh = node->mesh;
		for(uint32_t t = 0; t < mesh->primitives_count; t++)
		{
			memset(&CreateInfo, 0, sizeof(CreateInfo));
			CreateInfo.Color = { 1.0f, 1.0f, 1.0f };
			CreateInfo.Model = nodematrix;

			cgltf_primitive* prim = &mesh->primitives[t];
			cgltf_attribute* attr;

     			if(prim->attributes_count != VOID_VATTR_COUNT)
     			{
				printf("GLTF Load: unsupported vertex format: %s\n", SceneFile);
			     	return EXIT_FAILURE;
    			}

			if(prim->indices)
			{
				count = prim->indices->count;
				stride = prim->indices->stride;
				relbufferoffset = prim->indices->offset;
				absbufferoffset = prim->indices->buffer_view->offset;
				GLint indextype = 0; 
				if(prim->indices->component_type == cgltf_component_type_r_8u) 
				{
					indextype = GL_UNSIGNED_BYTE;
					EBOPadding = 4 - (count % 4);
					EBOPadding = (EBOPadding == 4) ? 0 : EBOPadding;
				}
				if(prim->indices->component_type == cgltf_component_type_r_16u) 
				{
					indextype =GL_UNSIGNED_SHORT;
					EBOPadding = (count % 2) * 2;
				}
				if(prim->indices->component_type == cgltf_component_type_r_32u) 
				{
					indextype = GL_UNSIGNED_INT;
					EBOPadding = 0;
				}
				if(indextype == 0)
				{
					printf("GLTF Load: No GL-compatible index type: %s", SceneFile);
					return EXIT_FAILURE;
				}

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, OffsetEBO, count * stride, (void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

				CreateInfo.IndexType = indextype;
				CreateInfo.IndexCount = count;
				CreateInfo.ByteOffsetEBO = OffsetEBO;

				// Since this is a shared buffer for the entire scene, pad all writes to nearest 32bit boundary to prevent misalignment when indextype changes from a smaller type to a larger one
				OffsetEBO += (count * stride) + EBOPadding;
			}

			attr = &prim->attributes[0]; 
			count = attr->data->count;
			relbufferoffset = attr->data->offset;
			absbufferoffset = attr->data->buffer_view->offset;

			stride = attr->data->stride;
			if((stride != VOID_VATTR_STRIDE) || 
			  (strcmp(attr->name, "POSITION") != 0) ||
			  (strcmp(prim->attributes[1].name, "NORMAL") != 0) ||
			  (strcmp(prim->attributes[2].name, "TEXCOORD_0") != 0))
			{
				printf("GLTF Load: unsupported vertex layout: %s\n", SceneFile);
				return EXIT_FAILURE;
			}

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, OffsetVBO * stride, count * stride, (void*)(DataBaseAddr + absbufferoffset + relbufferoffset));

			CreateInfo.VAttrCount = count;
			CreateInfo.OffsetVBO = OffsetVBO;

			// No need to pad this offset, because we're not mixing vertex formats within a single buffer
			OffsetVBO += count;

			WinHND->GeometryObjects.Alloc(CreateInfo);
		}
	}
	
	// TODO: switch to glDrawElementsIndirectCommand
	// /!\ remember to make changes where the draw calls actually happen too

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VOID_VATTR_STRIDE, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VOID_VATTR_STRIDE, (void*)12);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VOID_VATTR_STRIDE, (void*)24);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	cgltf_free(data);

//------------------------------------------------------------------------------------------------------------

	// Initialize Render passes
	
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

	WinHND->Projection = glm::perspective(glm::radians(VOID_HFOV_DEFAULT), SCREEN_X_DIM_DEFAULT / SCREEN_Y_DIM_DEFAULT, 0.1f, 100.0f);

	glm::mat4 Model = {};
	glm::vec3 LightPosition = { 1.2f, 1.0f, 2.0f };
	glm::vec3 lightScale = {0.2f, 0.2f, 0.2f};

	float CurrFrameTime = 0;
	float FrameEndTime = 0;
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
		CurrFrameTime = glfwGetTime();

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

		glBindVertexArray(VAO);

		// Mouse Picking Pass

		WinHND->PickPass.Bind_W();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		WinHND->PickShader.Use();

		glUniformMatrix4fv(pickingprojection_uni, 1, GL_FALSE, glm::value_ptr(WinHND->Projection));
		glUniformMatrix4fv(pickingview_uni, 1, GL_FALSE, glm::value_ptr(WinHND->View));

		for (unsigned int i = 0; i < WinHND->GeometryObjects.Position; i++)
		{
			if (WinHND->GeometryObjects.Visible[i] == VIS_STATUS_FREED)
			{
				continue;
			}

			glUniform1f(pickingindex_uni, float(i + 1));
			glUniform1f(pickingtype_uni, float(1));

			glUniformMatrix4fv(pickingmodel_uni, 1, GL_FALSE, glm::value_ptr(WinHND->GeometryObjects.Model[i]));
			
			if(WinHND->GeometryObjects.IndexCount[i])
			{
				glDrawElementsBaseVertex(RenderMode, WinHND->GeometryObjects.IndexCount[i], 
							 WinHND->GeometryObjects.IndexType[i], 
			    				 (void*)WinHND->GeometryObjects.ByteOffsetEBO[i], 
				    			 WinHND->GeometryObjects.OffsetVBO[i]);
			}
			else
			{
				glDrawArrays(RenderMode, WinHND->GeometryObjects.OffsetVBO[i], WinHND->GeometryObjects.VAttrCount[i]);
			}
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

		glUniformMatrix4fv(projection_uni, 1, GL_FALSE, glm::value_ptr(WinHND->Projection));

		glUniformMatrix4fv(view_uni, 1, GL_FALSE, glm::value_ptr(WinHND->View));
		glUniform3f(viewpos_uni, WinHND->Camera.Position.x, WinHND->Camera.Position.y, WinHND->Camera.Position.z);

		for (unsigned int i = 0; i < WinHND->GeometryObjects.Position; i++)
		{
			if (WinHND->GeometryObjects.Visible[i] == VIS_STATUS_FREED)
			{
				continue;
			}

			glUniformMatrix4fv(model_uni, 1, GL_FALSE, glm::value_ptr(WinHND->GeometryObjects.Model[i]));
			glUniform3fv(objcolor_uni, 1, glm::value_ptr(WinHND->GeometryObjects.Color[i]));

			if(WinHND->GeometryObjects.IndexCount[i])
			{
				glDrawElementsBaseVertex(RenderMode, WinHND->GeometryObjects.IndexCount[i], 
							 WinHND->GeometryObjects.IndexType[i], 
			    				 (void*)WinHND->GeometryObjects.ByteOffsetEBO[i], 
				    			 WinHND->GeometryObjects.OffsetVBO[i]);
			}
			else
			{
				glDrawArrays(RenderMode, WinHND->GeometryObjects.OffsetVBO[i], WinHND->GeometryObjects.VAttrCount[i]);
			}
		}

		if (WinHND->ActiveSelection)
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

		if(WinHND->GeometryObjects.IndexCount[0])
		{
			glDrawElementsBaseVertex(RenderMode, WinHND->GeometryObjects.IndexCount[0], 
			    WinHND->GeometryObjects.IndexType[0], 
			    (void*)WinHND->GeometryObjects.ByteOffsetEBO[0], 
			    WinHND->GeometryObjects.OffsetVBO[0]);
		}
		else
	{
			glDrawArrays(RenderMode, WinHND->GeometryObjects.OffsetVBO[0], WinHND->GeometryObjects.VAttrCount[0]);
		}

		// Blit from HDR to linear normal quad for post-processing, parse inter-frame data

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, WinHND->HDRPass.ColorBuffer);

		WinHND->PostShader.Use();
		glUniform1f(exposure_uni, float(exposure_val));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 		// Attributes hard coded in vertex shader

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		FrameEndTime = glfwGetTime();
		WinHND->FrameTimeMS = (FrameEndTime - CurrFrameTime) * 1000.0f;

		glfwSwapBuffers(Window);

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

	// Also resize camera frustum and all existing framebuffers
	WinHND->Projection = glm::perspective(glm::radians(VOID_HFOV_DEFAULT), (float)width / (float)height, 0.1f, 100.0f);
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
	WinHND->Camera.RelativeXAxis = glm::normalize(glm::cross(WinHND->Camera.Eye, WinHND->Camera.UpAxis));
	WinHND->Camera.RelativeYAxis = glm::normalize(glm::cross(WinHND->Camera.RelativeXAxis, WinHND->Camera.Eye));
	WinHND->Camera.Move(Window);
	WinHND->View = glm::lookAt(WinHND->Camera.Position, WinHND->Camera.Position + WinHND->Camera.Eye, WinHND->Camera.UpAxis);

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
			/*
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
			*/
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
		ImGui::Spacing();
		ImGui::Text("Use LShift to move down. Use Spacebar to move up");
		ImGui::Spacing();
		ImGui::Text("Hold Right Mouse button to control camera with mouse movement");
		ImGui::Spacing();
		ImGui::Text("UI elements capture mouse commands when hovered.");
		ImGui::Text("Move the mouse outside the menu to interact with the scene");
		ImGui::Spacing();
		if (ImGui::Button("Close"))
		{
			*HelpWindow = false;
		}

		ImGui::End();
	}
#ifdef DEBUG
	else if (*DemoWindow)
	{
		ImGui::ShowDemoWindow(DemoWindow);
	}
#endif
	ImGui::Begin("Scene Controls");

	ImGui::Spacing();
	if (ImGui::Button("New Object (n)"))
	{
		WinHND->Active.New = true;
	}
	ImGui::Spacing();
	if (ImGui::Button("Post-Processing"))
	{
		*PostWindow = true;
	}
	ImGui::Spacing();
	if (ImGui::Button("Reload Shaders (p)"))
	{
		WinHND->ReloadShaders = true;
	}
	ImGui::Spacing();
	if (ImGui::Button("Help"))
	{
		*HelpWindow = true;
	}
#ifdef DEBUG
	ImGui::SameLine();
	if (ImGui::Button("Show Demo Window"))
	{
		*DemoWindow = true;
	}
#endif
	ImGui::Spacing();
	if (ImGui::Button("Exit (esc)"))
	{
		WinHND->ShouldExit = true;
	}
	ImGui::Spacing();
	ImGui::Text("Frame time: %.2f ms", WinHND->FrameTimeMS);
	ImGui::Spacing();
	ImGui::Text("Display rate: %.2f ms", WinHND->DeltaTime * 1000.0f);

	ImGui::End();
}

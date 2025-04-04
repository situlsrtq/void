#include "../inc/imgui/imgui.h"
#include "../inc/imgui/imgui_impl_glfw.h"
#include "../inc/imgui/imgui_impl_opengl3.h"
#include "../inc/glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.h"
#include "picking.h"
#include "camera.h"
#include "window.h"
#include "util/u_math.h"
#include "util/u_mem.h"


#define SCREEN_X_DIM 800.0f
#define SCREEN_Y_DIM 600.0f


void FrameResizeCallback(GLFWwindow* Window, int width, int height);
void MousePosCallback(GLFWwindow* Window, double mx, double my);
void ProcessInput(GLFWwindow* Window);

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


int main(void)
{

// Initialize Core Systems

    if (!glfwInit())
        return -1;

	const char* GLSLVersion = "#version 460";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow * Window = glfwCreateWindow(800,600,"mBox",0,0);
	if(!Window)
	{	
		printf("GLFW: Failed to create window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(Window);
	glfwSetFramebufferSizeCallback(Window, FrameResizeCallback);

	// /!\ gladLoadGLLoader() overwrites all gl functions, can only be called after successfully setting a current context 

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD: Failed getting function pointers\n");
		return -1;
	}

	window_handler_t* WinHND = InitWindowHandler(SCREEN_X_DIM, SCREEN_Y_DIM);
	if (!WinHND)
	{
		printf("System: Could not allocate core window handler\n");
		return -1;
	}
	glfwSetWindowUserPointer(Window, (void *)WinHND);
	
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(Window, MousePosCallback);

// Initialize ImGui Context

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	WinHND->ImIO = ImGui::GetIO(); (void)WinHND->ImIO;
	WinHND->ImIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();

	ImGuiStyle& UIStyle = ImGui::GetStyle();
	if (WinHND->ImIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		UIStyle.WindowRounding = 0.0f;
		UIStyle.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	bool uisuccess = false;
	uisuccess = ImGui_ImplGlfw_InitForOpenGL(Window, true);
	if (!uisuccess)
	{
		printf("System: Could not initialize imgui context for GLFW\n");
		return -1;
	}
	uisuccess = ImGui_ImplOpenGL3_Init(GLSLVersion);
	if (!uisuccess)
	{
		printf("System: Could not initialize imgui context for OpenGL\n");
		return -1;
	}

	float CubeMesh[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f };

	uMATH::vec3f_t cubePositions[] = {
		uMATH::vec3f_t{ 0.0f,  0.0f,  0.0f},
		uMATH::vec3f_t{ 2.0f,  5.0f, -15.0f},
		uMATH::vec3f_t{-1.5f, -2.2f, -2.5f},
		uMATH::vec3f_t{-3.8f, -2.0f, -12.3f},
		uMATH::vec3f_t{ 2.4f, -0.4f, -3.5f},
		uMATH::vec3f_t{-1.7f,  3.0f, -7.5f},
		uMATH::vec3f_t{ 1.3f, -2.0f, -2.5f},
		uMATH::vec3f_t{ 1.5f,  2.0f, -2.5f},
		uMATH::vec3f_t{ 1.5f,  0.2f, -1.5f},
		uMATH::vec3f_t{-1.3f,  1.0f, -1.5f}
	};

// Initialize Core VBO, VAO, Render passes

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);
#endif

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeMesh), CubeMesh, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	int success = WinHND->MainShader.Create("../shaders/main.vert", "../shaders/main.frag");
	if (success != 0)
	{
		printf("System: Failed to initialize main pass shaders\n");
		return -1;
	}

	unsigned int model_uni = glGetUniformLocation(WinHND->MainShader.ID, "model");
	unsigned int view_uni = glGetUniformLocation(WinHND->MainShader.ID, "view");
	unsigned int viewpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "viewpos");
	unsigned int projection_uni = glGetUniformLocation(WinHND->MainShader.ID, "projection");
	unsigned int lightpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "lightpos");
	unsigned int objcolor_uni = glGetUniformLocation(WinHND->MainShader.ID, "objcolor");
	unsigned int lloc = glGetUniformLocation(WinHND->MainShader.ID, "lightcolor");
	unsigned int ambistrgth_uni = glGetUniformLocation(WinHND->MainShader.ID, "ambientstrength");

	success = WinHND->PickPass.Init(WinHND->Width, WinHND->Height);
	if (success != 0)
	{
		printf("System: Failed to initialize pick pass framebuffer\n");
		return -1;
	}

	success = WinHND->PickShader.Create("../shaders/pick.vert", "../shaders/pick.frag");
	if (success != 0)
	{
		printf("System: Failed to initialize pick pass shaders\n");
		return -1;
	}

	unsigned int pickingmodel_uni = glGetUniformLocation(WinHND->PickShader.ID, "model");
	unsigned int pickingview_uni = glGetUniformLocation(WinHND->PickShader.ID, "view");
	unsigned int pickingprojection_uni = glGetUniformLocation(WinHND->PickShader.ID, "projection");
	unsigned int pickingindex_uni = glGetUniformLocation(WinHND->PickShader.ID, "index");
	unsigned int pickingtype_uni = glGetUniformLocation(WinHND->PickShader.ID, "type");

// Initialize first-frame data

	uMATH::mat4f_t GeometryModel = {};
	geometry_create_info_t CreateInfo;
	CreateInfo.Scale = 1.0f;
	CreateInfo.Intensity = 0.5f;
	CreateInfo.Color = { 1.0f, 0.5f, 0.31f };

	for (int i = 0; i < 10; i++)
	{
		float angle = 20.0f * i;
		uMATH::vec3f_t rVec = { 1.0f, 0.3f, 0.5f };
		SetTransform(&GeometryModel);

		uMATH::Scale(&GeometryModel, CreateInfo.Scale);
		uMATH::MatrixRotate(&GeometryModel, angle, rVec);
		uMATH::Translate(&GeometryModel, cubePositions[i]);

		CreateInfo.Model = GeometryModel;
		WinHND->GeometryObjects.Alloc(CreateInfo);
	}

	uMATH::SetFrustumHFOV(&WinHND->Projection, 45.0f, SCREEN_X_DIM / SCREEN_Y_DIM, 0.1f, 100.0f);

	uMATH::mat4f_t Model = {};
	uMATH::vec3f_t LightPosition = { 1.2f, 1.0f, 2.0f };
	float lightScale = 0.2f;

	float CurrFrameTime = 0;

	glEnable(GL_DEPTH_TEST);
	int RenderMode = GL_TRIANGLES;
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

// Frame loop

	bool showwindow = true;
	uMATH::vec3f_t Color = {};
	while (!glfwWindowShouldClose(Window))
	{

// Input

		glfwPollEvents();
		WinHND->ImIO = ImGui::GetIO();
		WinHND->ImIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ProcessInput(Window);

// UI Framegen

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if(showwindow)
		{
			ImGui::ShowDemoWindow(&showwindow);
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Object Parameters");

			ImGui::Checkbox("Demo Window", &showwindow);
			ImGui::Checkbox("Another Window", &showwindow);
			ImGui::Text("");
			ImGui::SliderFloat("Scale", &f, 0.1f, 20.0f);
			ImGui::ColorEdit3("Color", (float*)&Color);

			ImGui::Text("Frame time: %.3f ms/frame (%.1f FPS)", WinHND->DeltaTime, 1.0f / WinHND->DeltaTime);
			ImGui::End();
		}
//Render

	//-----------------------------------Mouse Picking Pass----------------------------------------

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
			glDrawArrays(RenderMode, 0, 36);
		}

		WinHND->PickPass.Unbind_W();

	//------------------------------------Object Geometry Pass---------------------------------------

		WinHND->MainShader.Use();

		glUniform1f(ambistrgth_uni, 0.1f);
		glUniform3f(objcolor_uni, 1.0f, 0.5f, 0.31f);
		glUniform3f(lloc, 1.0f, 1.0f, 1.0f);
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
			glDrawArrays(RenderMode, 0, 36);
		}

		if (WinHND->ActiveSelection == 1)
		{
			WinHND->GeometryObjects.Alloc(WinHND->Active);
			glUniformMatrix4fv(model_uni, 1, GL_FALSE, &WinHND->Active.Model.m[0][0]);
			glDrawArrays(RenderMode, 0, 36);
			WinHND->ActiveSelection = 0;
		}

	//----------------------------------Light Geometry Pass----------------------------------------

		glUniform1f(ambistrgth_uni, 1.0f);
		glUniform3f(objcolor_uni, 1.0f, 1.0f, 1.0f);
		SetTransform(&Model);
		uMATH::Scale(&Model, lightScale);
		uMATH::Translate(&Model, LightPosition);
		glUniformMatrix4fv(model_uni, 1, GL_FALSE, &Model.m[0][0]);
		glDrawArrays(RenderMode, 0, 36);

		glBindVertexArray(0);
		glUseProgram(0);

// Blit, parse inter-frame data

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (WinHND->ImIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(context);
		}

		glfwSwapBuffers(Window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CurrFrameTime = glfwGetTime();
		WinHND->DeltaTime = CurrFrameTime - WinHND->PrevFrameTime;
		WinHND->PrevFrameTime = CurrFrameTime;
	}

// Free resources and exit

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glfwTerminate();
	free(WinHND);
	WinHND = 0x0;

    return 0;
}


void FrameResizeCallback(GLFWwindow *Window, int width, int height)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	WinHND->Width = width;
	WinHND->Height = height;

	// Also resize camera frustum and attached framebuffers

	uMATH::SetFrustumHFOV(&WinHND->Projection, 45.0f, width / height, 0.1f, 100.0f);
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

	if(glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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
		if (PKeyWasDown)
		{
			WinHND->MainShader.Rebuild();
			unsigned int model_uni = glGetUniformLocation(WinHND->MainShader.ID, "model");
			unsigned int view_uni = glGetUniformLocation(WinHND->MainShader.ID, "view");
			unsigned int viewpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "viewpos");
			unsigned int projection_uni = glGetUniformLocation(WinHND->MainShader.ID, "projection");
			unsigned int lightpos_uni = glGetUniformLocation(WinHND->MainShader.ID, "lightpos");
			unsigned int objcolor_uni = glGetUniformLocation(WinHND->MainShader.ID, "objcolor");
			unsigned int lloc = glGetUniformLocation(WinHND->MainShader.ID, "lightcolor");
			unsigned int ambistrgth_uni = glGetUniformLocation(WinHND->MainShader.ID, "ambientstrength");

			PKeyWasDown = 0;
		}
	}
	if (glfwGetKey(Window, GLFW_KEY_N) == GLFW_PRESS)
	{
		NKeyWasDown = 1;
	}
	if (glfwGetKey(Window, GLFW_KEY_N) == GLFW_RELEASE)
	{
		if (NKeyWasDown)
		{
			uMATH::vec3f_t p = { 0.0f,0.0f,4.5f };
			WinHND->Active.Model = WinHND->View;
			uMATH::Translate(&WinHND->Active.Model, p);
			WinHND->Active.Model = uMATH::InverseM4(WinHND->Active.Model);
			WinHND->ActiveSelection = 1;
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
			printf("Index: %f | Type: %f\n", res.ID, res.Type);
			if (res.ID > 0)
			{
				WinHND->GeometryObjects.Free(res.ID - 1);
			}
		}

		LMouseWasDown = 0;
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

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
void ProcessInput(GLFWwindow* Window);
void MousePosCallback(GLFWwindow* Window, double mx, double my);

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


int main(void)
{

// Initialize Core Systems

    if (!glfwInit())
        return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow * Window = glfwCreateWindow(800,600,"TestPlatform",0,0);
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

	float VertexData[] = {
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

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);
#endif

// Initialize Core VBO, VAO, Render passes

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
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

	while (!glfwWindowShouldClose(Window))
	{

// Input

		glfwPollEvents();
		ProcessInput(Window);

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

// Blit, pasrse inter-frame data

		glfwSwapBuffers(Window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CurrFrameTime = glfwGetTime();
		WinHND->DeltaTime = CurrFrameTime - WinHND->PrevFrameTime;
		WinHND->PrevFrameTime = CurrFrameTime;
	}

// Free resources and exit

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

	uMATH::SetFrustumHFOV(&WinHND->Projection, 45.0f, width / height, 0.1f, 100.0f);
	WinHND->PickPass.Release();
	WinHND->PickPass.Init(width, height);

	glViewport(0,0,width,height);
}


void ProcessInput(GLFWwindow *Window)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

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
}


void MousePosCallback(GLFWwindow *Window, double mx, double my)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

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

	WinHND->Camera.LookAtMouse(xoffset, yoffset);
}

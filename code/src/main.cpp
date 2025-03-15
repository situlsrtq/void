#include "../inc/glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.h"
#include "camera.h"
#include "window.h"
#include "util/u_math.h"
#include "util/u_mem.h"
#include "util/u_phys.h"


#define SCREEN_X_DIM 800
#define SCREEN_Y_DIM 600


void FrameResizeCallback(GLFWwindow* Window, int width, int height);
void ProcessInput(GLFWwindow* Window);
void MousePosCallback(GLFWwindow* Window, double mx, double my);

uint8_t NKeyWasDown;
uint8_t RKeyWasDown;
uint8_t LMouseWasDown;

int main(void)
{

// Initialize GLFW

    if (!glfwInit())
        return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	GLFWwindow * Window = glfwCreateWindow(800,600,"TestPlatform",0,0);
	if(!Window)
	{	
		printf("GLFW: Failed to create window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(Window);
	glfwSetFramebufferSizeCallback(Window, FrameResizeCallback);

// Load GLAD functions - overwrites all gl functions, can only be called after setting context

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

	WinHND->Shader.Create("../shaders/test.vert", "../shaders/test.frag");

	unsigned int model_uni = glGetUniformLocation(WinHND->Shader.ID, "model");
	unsigned int view_uni = glGetUniformLocation(WinHND->Shader.ID, "view");
	unsigned int viewpos_uni = glGetUniformLocation(WinHND->Shader.ID, "viewpos");
	unsigned int projection_uni = glGetUniformLocation(WinHND->Shader.ID, "projection");
	unsigned int lightpos_uni = glGetUniformLocation(WinHND->Shader.ID, "lightpos");
	unsigned int objcolor_uni = glGetUniformLocation(WinHND->Shader.ID, "objcolor");
	unsigned int lloc = glGetUniformLocation(WinHND->Shader.ID, "lightcolor");
	unsigned int ambistrgth_uni = glGetUniformLocation(WinHND->Shader.ID, "ambientstrength");

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

	uMATH::SetFrustumHFOV(&WinHND->Projection, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
	WinHND->InverseProjection = InverseM4(WinHND->Projection);							// Projection matrix is essentially static so precalculate

	uMATH::mat4f_t Model = {};
	uMATH::vec3f_t LightPosition = { 1.2f, 1.0f, 2.0f };
	float lightScale = 0.2f;

	float CurrFrameTime = 0;

	glEnable(GL_DEPTH_TEST);
	int RenderMode = GL_TRIANGLES;
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	while (!glfwWindowShouldClose(Window))
	{

// Input

		glfwPollEvents();
		ProcessInput(Window);

//Render

		WinHND->Shader.Use();

	//------------------------------------Draw Objects
		glUniform1f(ambistrgth_uni, 0.1f);
		glUniform3f(objcolor_uni, 1.0f, 0.5f, 0.31f);
		glUniform3f(lloc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightpos_uni, LightPosition.x, LightPosition.y, LightPosition.z);

		glUniformMatrix4fv(projection_uni, 1, GL_FALSE, &WinHND->Projection.m[0][0]);

		glUniformMatrix4fv(view_uni, 1, GL_FALSE, &WinHND->View.m[0][0]);
		glUniform3f(viewpos_uni, WinHND->Camera.Position.x, WinHND->Camera.Position.y, WinHND->Camera.Position.z);
		
		glBindVertexArray(VAO);
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

	//----------------------------------Draw Light Sources
		glUniform1f(ambistrgth_uni, 1.0f);
		glUniform3f(objcolor_uni, 1.0f, 1.0f, 1.0f);
		SetTransform(&Model);
		uMATH::Scale(&Model, lightScale);
		uMATH::Translate(&Model, LightPosition);
		glUniformMatrix4fv(model_uni, 1, GL_FALSE, &Model.m[0][0]);
		glDrawArrays(RenderMode, 0, 36);

		glBindVertexArray(0);
		glUseProgram(0);

// Blit, Upkeep data

		glfwSwapBuffers(Window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CurrFrameTime = glfwGetTime();
		WinHND->DeltaTime = CurrFrameTime - WinHND->PrevFrameTime;
		WinHND->PrevFrameTime = CurrFrameTime;
	}

    glfwTerminate();
    return 0;
}


void FrameResizeCallback(GLFWwindow *Window, int width, int height)
{
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);
	WinHND->Width = width;
	WinHND->Height = height;
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
	if(glfwGetKey(Window, GLFW_KEY_P) == GLFW_PRESS)
	{
		WinHND->Shader.Rebuild();
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
	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
	{
		RKeyWasDown = 1;
	}
	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_RELEASE)
	{
		if (RKeyWasDown)
		{
		}
		RKeyWasDown = 0;
	}
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		LMouseWasDown = 1;
	}
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		if(LMouseWasDown)
		{
			int xpos, ypos;
			glfwGetWindowPos(Window, &xpos, &ypos);
			uMATH::vec3f_t ray = uPHYS::CastWorldRay((float)xpos, (float) ypos, *WinHND);

			uMATH::vec3f_t minAABB = { -1.0f,-1.0f,-1.0f };
			uMATH::vec3f_t maxAABB = { 1.0f, 1.0f, 1.0f };
			uMATH::vec3f_t origin = { WinHND->Camera.Position.x,WinHND->Camera.Position.y,WinHND->Camera.Position.z };
			float distance = 0.0f;
			float dmax = 100000.0f;
			uint8_t foundindex = 0;
			bool ret = false;
			bool found = false;

			for (int i = 0; i < WinHND->GeometryObjects.Position; i++)
			{
				ret = uPHYS::CheckRayOBBCollision(origin, ray, minAABB, maxAABB, WinHND->GeometryObjects.Model[i], &distance);
				if (ret)
				{
					if (distance < dmax)
					{
						foundindex = i;
						dmax = distance;
						found = true;
					}
				}
			}

			if (found)
			{
				WinHND->GeometryObjects.Free(foundindex);
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

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <cmath>

#include "shader.h"
#include "../include/util/u_math.h"


//-----------------------------SYSTEM VALUES----------------------------------
float DeltaTime = 0.0f;
float PrevFrameTime = 0.0f;
int FirstFrame = 1;
//----------------------------------------------------------------------------


// ----------------------------CAMERA VALUES----------------------------------
uMATH::vec3f_t CameraPosition = { 0.0f, 0.0f, 3.0f };
uMATH::vec3f_t CameraFront = { 0.0f, 0.0f, -1.0f };
uMATH::vec3f_t CameraUpAxis = { 0.0f, 1.0f, 0.0f };

float MouseSensitivity = 0.1f;
float PrevMouseX = 800.0f/ 2.0f;
float PrevMouseY = 600.0f/ 2.0f;
float Yaw = -90.0f;
float Pitch = 0.0f;
//----------------------------------------------------------------------------


void FrameResizeCallback(GLFWwindow *Window, int width, int height)
{
	glViewport(0,0,width,height);
}


void ProcessInput(GLFWwindow *Window)
{
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
//--------------------------------CAMERA MOVEMENT----------------------------------
	float CameraSpeed = 2.5f * DeltaTime;
	uMATH::vec3f_t CameraRelativeXAxis = uMATH::Normalize(uMATH::Cross(CameraFront, CameraUpAxis));
	uMATH::vec3f_t CameraRelativeYAxis = uMATH::Normalize(uMATH::Cross(CameraRelativeXAxis, CameraFront));

	if(glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		CameraPosition += Scalar(CameraFront, CameraSpeed);
	}
	if(glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		CameraPosition -= Scalar(CameraFront, CameraSpeed);
	}
	if(glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		CameraPosition -= Scalar(CameraRelativeXAxis, CameraSpeed);
	}
	if(glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		CameraPosition += Scalar(CameraRelativeXAxis, CameraSpeed);
	}
	if(glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		CameraPosition += Scalar(CameraRelativeYAxis, CameraSpeed);
	}
	if(glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		CameraPosition -= Scalar(CameraRelativeYAxis, CameraSpeed);
	}
//---------------------------------------------------------------------------------
}


void MousePosCallback(GLFWwindow *Window, double MouseX, double MouseY)
{
	if (FirstFrame)
	{
		PrevMouseX = MouseX;
		PrevMouseY = MouseY;
		FirstFrame = 0;
	}
	
	float xoffset = MouseX - PrevMouseX;
	float yoffset = PrevMouseY - MouseY;
	
	PrevMouseX = MouseX;
	PrevMouseY = MouseY;

	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (Pitch > 89.0f)
	{
		Pitch = 89.0f;
	}
	if (Pitch < -89.0f)
	{
		Pitch = -89.0f;
	}

	uMATH::vec3f_t camdirection = {};
	camdirection.x = cos((Yaw * RADIAN)) * cos((Pitch * RADIAN));
	camdirection.y = sin((Pitch * RADIAN));
	camdirection.z = sin((Yaw * RADIAN)) * cos((Pitch * RADIAN));

	CameraFront = uMATH::Normalize(camdirection);
}


int main(void)
{

// Initialize GLFW

    if (!glfwInit())
        return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
	
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(Window, MousePosCallback);

	float Vertices[] = {
	-0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f, 
	 0.5f,  0.5f, -0.5f, 
	 0.5f,  0.5f, -0.5f, 
	-0.5f,  0.5f, -0.5f, 
	-0.5f, -0.5f, -0.5f, 

	-0.5f, -0.5f,  0.5f, 
	 0.5f, -0.5f,  0.5f, 
	 0.5f,  0.5f,  0.5f, 
	 0.5f,  0.5f,  0.5f, 
	-0.5f,  0.5f,  0.5f, 
	-0.5f, -0.5f,  0.5f, 

	-0.5f,  0.5f,  0.5f, 
	-0.5f,  0.5f, -0.5f, 
	-0.5f, -0.5f, -0.5f, 
	-0.5f, -0.5f, -0.5f, 
	-0.5f, -0.5f,  0.5f, 
	-0.5f,  0.5f,  0.5f, 

	 0.5f,  0.5f,  0.5f, 
	 0.5f,  0.5f, -0.5f, 
	 0.5f, -0.5f, -0.5f, 
	 0.5f, -0.5f, -0.5f, 
	 0.5f, -0.5f,  0.5f, 
	 0.5f,  0.5f,  0.5f, 

	-0.5f, -0.5f, -0.5f, 
	 0.5f, -0.5f, -0.5f, 
	 0.5f, -0.5f,  0.5f, 
	 0.5f, -0.5f,  0.5f, 
	-0.5f, -0.5f,  0.5f, 
	-0.5f, -0.5f, -0.5f, 

	-0.5f,  0.5f, -0.5f, 
	 0.5f,  0.5f, -0.5f, 
	 0.5f,  0.5f,  0.5f, 
	 0.5f,  0.5f,  0.5f, 
	-0.5f,  0.5f,  0.5f, 
	-0.5f,  0.5f, -0.5f, };

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	shader_t Shader;
	Shader.Build("../shaders/test.vert", "../shaders/test.frag");
	int RenderMode = GL_TRIANGLES;

	uMATH::mat4f_t Projection = {};
	uMATH::SetFrustumHFOV(&Projection, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

	unsigned int mloc = glGetUniformLocation(Shader.ID, "model");
	unsigned int vloc = glGetUniformLocation(Shader.ID, "view");
	unsigned int ploc = glGetUniformLocation(Shader.ID, "projection");

	uMATH::mat4f_t View = {};
	uMATH::mat4f_t Model = {};
	float CurrFrameTime = 0;

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	while (!glfwWindowShouldClose(Window))
	{

// Input

		glfwPollEvents();
		ProcessInput(Window);

//Render

		Shader.Use();

		glUniformMatrix4fv(ploc, 1, GL_FALSE, &Projection.m[0][0]);

		uMATH::SetCameraView(&View, CameraPosition, CameraPosition+CameraFront, CameraUpAxis);
		glUniformMatrix4fv(vloc, 1, GL_FALSE, &View.m[0][0]);
		
		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			float angle = 20.0f * i;
			SetTransform(&Model);

			uMATH::Translate(&Model, cubePositions[i]);
			uMATH::vec3f_t rVec = { 1.0f, 0.3f, 0.5f };
			uMATH::MatrixRotate(&Model, angle, rVec);
			glUniformMatrix4fv(mloc, 1, GL_FALSE, &Model.m[0][0]);

			glDrawArrays(RenderMode, 0, 36);
		}

		glBindVertexArray(0);

// Blit

		glfwSwapBuffers(Window);
//		glClearColor(0.42f, 0.40f, 0.38f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CurrFrameTime = glfwGetTime();
		DeltaTime = CurrFrameTime - PrevFrameTime;
		PrevFrameTime = CurrFrameTime;
	}

    glfwTerminate();
    return 0;
}

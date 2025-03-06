#include "../inc/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <cmath>

#include "shader.h"
#include "camera.h"
#include "../inc/util/u_math.h"


//-----------------------------SYSTEM VALUES----------------------------------
float DeltaTime = 0.0f;
float PrevFrameTime = 0.0f;
int FirstFrame = 1;
double PrevMouseX = 800.0f / 2.0f;
double PrevMouseY = 600.0f / 2.0f;

mbox_camera_t Camera;
//----------------------------------------------------------------------------


void FrameResizeCallback(GLFWwindow *Window, int width, int height)
{
	glViewport(0,0,width,height);
}


void ProcessInput(GLFWwindow *Window, mbox_camera_t *Camera)
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

	Camera->Speed = 2.5f * DeltaTime;
	Camera->RelativeXAxis = uMATH::Normalize(uMATH::Cross(Camera->Eye, Camera->UpAxis));
	Camera->RelativeYAxis = uMATH::Normalize(uMATH::Cross(Camera->RelativeXAxis, Camera->Eye));
	Camera->Move(Window);
}


void MousePosCallback(GLFWwindow *Window, double mx, double my)
{
	if(FirstFrame)
	{
		PrevMouseX = mx;
		PrevMouseY = my;
		FirstFrame = 0;
	}

	float xoffset = mx - PrevMouseX;
	float yoffset = PrevMouseY - my;
	
	PrevMouseX = mx;
	PrevMouseY = my;

	Camera.LookAtMouse(xoffset, yoffset);
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

	glEnable(GL_DEPTH_TEST);

	shader_t Shader;
	Shader.Build("../shaders/test.vert", "../shaders/test.frag");
	int RenderMode = GL_TRIANGLES;

	unsigned int mloc = glGetUniformLocation(Shader.ID, "model");
	unsigned int vloc = glGetUniformLocation(Shader.ID, "view");
	unsigned int ploc = glGetUniformLocation(Shader.ID, "projection");
	unsigned int lploc = glGetUniformLocation(Shader.ID, "lightpos");
	unsigned int cloc = glGetUniformLocation(Shader.ID, "objcolor");
	unsigned int lloc = glGetUniformLocation(Shader.ID, "lightcolor");
	unsigned int aloc = glGetUniformLocation(Shader.ID, "ambientstrength");
	unsigned int vdloc = glGetUniformLocation(Shader.ID, "viewpos");

	uMATH::mat4f_t View = {};
	uMATH::mat4f_t Model = {};
	uMATH::mat4f_t Projection = {};
	uMATH::SetFrustumHFOV(&Projection, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

	uMATH::vec3f_t LightPosition = { 1.2f, 1.0f, 2.0f };
	float lightScale = 0.2f;

	float CurrFrameTime = 0;

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	while (!glfwWindowShouldClose(Window))
	{

// Input

		glfwPollEvents();
		ProcessInput(Window, &Camera);

//Render

		Shader.Use();

	//------------------------------------Draw Objects
		glUniform1f(aloc, 0.1f);
		glUniform3f(cloc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lloc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lploc, LightPosition.x, LightPosition.y, LightPosition.z);

		glUniformMatrix4fv(ploc, 1, GL_FALSE, &Projection.m[0][0]);

		uMATH::SetCameraView(&View, Camera.Position, Camera.Position+Camera.Eye, Camera.UpAxis);
		glUniformMatrix4fv(vloc, 1, GL_FALSE, &View.m[0][0]);
		glUniform3f(vdloc, Camera.Position.x, Camera.Position.y, Camera.Position.z);
		
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

	//----------------------------------Draw Light Sources
		glUniform1f(aloc, 1.0f);
		glUniform3f(cloc, 1.0f, 1.0f, 1.0f);
		SetTransform(&Model);
		uMATH::Translate(&Model, LightPosition);
		uMATH::Scale(&Model, lightScale);
		glUniformMatrix4fv(mloc, 1, GL_FALSE, &Model.m[0][0]);
		glDrawArrays(RenderMode, 0, 36);

		glBindVertexArray(0);

// Blit

		glfwSwapBuffers(Window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CurrFrameTime = glfwGetTime();
		DeltaTime = CurrFrameTime - PrevFrameTime;
		PrevFrameTime = CurrFrameTime;
	}

    glfwTerminate();
    return 0;
}

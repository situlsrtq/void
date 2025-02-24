#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <cmath>

#include "shader.h"
#include "../include/util/u_math.h"


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

	unsigned int Indices[] = { 
		0, 1, 3, 
		1, 2, 3 
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

	uMATH::mat4f_t Model = {};
	SetTransform(&Model);
// TODO: Rotation vector must be unit vector - guarantee in uMATH MatrixRotate
	uMATH::vec3f_t rVec = { 0.662f, 0.2f, 0.722f };

	uMATH::mat4f_t View = {};
	SetTransform(&View);
	uMATH::vec3f_t vVec = { 0.0f, 0.0f, -6.0f };
	uMATH::Translate(&View, vVec);

	uMATH::mat4f_t Projection = {};
	uMATH::SetFrustumHFOV(&Projection, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

	unsigned int mloc = glGetUniformLocation(Shader.ID, "model");
	unsigned int vloc = glGetUniformLocation(Shader.ID, "view");
	unsigned int ploc = glGetUniformLocation(Shader.ID, "projection");

	float time;

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	while (!glfwWindowShouldClose(Window))
	{

// Input

		glfwPollEvents();
		ProcessInput(Window);

//Render

		Shader.Use();
		uMATH::MatrixRotate(&Model, (float)glfwGetTime() * 0.9599f, rVec);
		glUniformMatrix4fv(mloc, 1, GL_FALSE, &Model.m[0][0]);
		glUniformMatrix4fv(vloc, 1, GL_FALSE, &View.m[0][0]);
		glUniformMatrix4fv(ploc, 1, GL_FALSE, &Projection.m[0][0]);
		glBindVertexArray(VAO);
		glDrawArrays(RenderMode,0,36);
		glBindVertexArray(0);

// Blit

		glfwSwapBuffers(Window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

    glfwTerminate();
    return 0;
}

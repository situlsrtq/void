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

// Load GLAD functions - overwrites all gl functions, can only be called after setting context

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD: Failed getting function pointers\n");
		return -1;
	}

	glfwSetFramebufferSizeCallback(Window, FrameResizeCallback);

	float Vertices[] = {
		0.5f, 0.5f, 0.0f, // top right
		0.5f,-0.5f, 0.0f, // bottom right
	   -0.5f,-0.5f, 0.0f, // bottom left
       -0.5f, 0.5f, 0.0f // top left
	};

	unsigned int Indices[] = { 
		0, 1, 3, 
		1, 2, 3 
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	shader_t Shader;
	Shader.Build("../shaders/test.vert", "../shaders/test.frag");
	int RenderMode = GL_TRIANGLES;

	mat4f_t Transform = {};
	SetTransform(&Transform);
	vec3f_t ScaleV = {1, 1, 1};
	vec3f_t TranslateV = {0, 0, 0};

	uint transloc = glGetUniformLocation(Shader.ID, "trans");
	float time;
	float step;

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	while(!glfwWindowShouldClose(Window))
	{

// Input

		SetTransform(&Transform);

		glfwPollEvents();
		ProcessInput(Window);

		time = glfwGetTime();
		step = sin(time);
		
		ScaleV.x = step;
		ScaleV.y = step;
//		ScaleV.z = step;
		TranslateV.x = step;
		TranslateV.y = step;
//		TranslateV.z = step;

		Translate(&Transform, &TranslateV);
		Rotate(&Transform, step*2, R_AXIS_Z);
		Scale(&Transform, &ScaleV);

//Render

		Shader.Use();
		glUniformMatrix4fv(transloc, 1, GL_FALSE, &Transform.m[0][0]);
		glBindVertexArray(VAO);
		glDrawElements(RenderMode,6,GL_UNSIGNED_INT,0);
		glBindVertexArray(0);

// Blit

		glfwSwapBuffers(Window);
		glClear(GL_COLOR_BUFFER_BIT);
	}

    glfwTerminate();
    return 0;
}

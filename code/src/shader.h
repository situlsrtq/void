#ifndef MBOX_SHADERS_H
#define MBOX_SHADERS_H


#include "../inc/glad/glad.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/u_math.h"


#define TMAX_PATH_LEN 1024
#define SHADER_INFOLOG_SIZE 512


struct shader_t
{
	uint32_t ID;
	char InfoLog[SHADER_INFOLOG_SIZE];
	char VertPath[TMAX_PATH_LEN];
	char FragPath[TMAX_PATH_LEN];

	uint32_t Build(const char *FilePath, int ShaderType);
	int Create(const char *VertPath, const char *FragPath);
	int Rebuild();
	void Use();

	/*
	SetUniform() needs a more complex implementation in order to avoid looking up known uniform locations every frame.
	Would require a sorted list of uniforms based on line-by-line parsing of the shader file. Would require data
	structure, but also the ability to detect if a uniform, as declared in the source, is valid _before_ compilation - 
	very cool problem, but way beyond the scope of this project

	int SetUniform(const char* Uniform, int Type, void *Data);
	*/
};


#endif


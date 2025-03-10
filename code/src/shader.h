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

	/* SetUniform() needs a more complex implementation in order to avoid looking up uniform locations every frame
	int SetUniform(const char* Uniform, int Type, void *Data);
	*/
};


#endif


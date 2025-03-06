#ifndef MBOX_SHADERS_H
#define MBOX_SHADERS_H


#include "../inc/glad/glad.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/util/u_math.h"


#define TMAX_PATH_LEN 1024


struct shader_t
{
	uint32_t ID;
	uint32_t VertexShader;
	uint32_t FragmentShader;
	char InfoLog[512];
	char VertPath[TMAX_PATH_LEN];
	char FragPath[TMAX_PATH_LEN];

	int Build(const char *VertPath, const char *FragPath);
	int Rebuild();
	void Use();
	int SetUniform(const char* Uniform, int Type, void *Data);
};


#endif


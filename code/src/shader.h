#ifndef TEST_SHADERS_H
#define TEST_SHADERS_H


#include "../include/glad/glad.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/util/u_math.h"


#define TMAX_PATH_LEN 1024


struct shader_t
{
	uint32_t ID;
	uint32_t VertexShader;
	uint32_t FragmentShader;
	char VertPath[1024];
	char FragPath[1024];

	int Build(const char *VertPath, const char *FragPath);
	int Rebuild();
	void Use();
	int SetUniform(const char* Uniform, int Type, void *Data);
};


#endif


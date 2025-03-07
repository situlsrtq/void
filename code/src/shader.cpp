#include "shader.h"


uint32_t shader_t::Build(const char* InFilePath, int ShaderType)
{
	size_t len = strlen(InFilePath) + 1;
	if (len > TMAX_PATH_LEN)
	{
		printf("Shader Error: file path longer than max\n");
		return -1;
	}

	switch (ShaderType)
	{
		case GL_VERTEX_SHADER:
			strcpy(VertPath, InFilePath);
			break;
		case GL_FRAGMENT_SHADER:
			strcpy(FragPath, InFilePath);
			break;
		default:
			printf("Shader Error: Shader type not supported\n");
			break;
	}

	char* FileSrc = 0x0;
	FILE* SFile = 0x0;
	uint64_t srclen = 0;
	int success = 0;

// Read shader file

	if ((SFile = fopen(InFilePath, "rb")) == 0x0)
	{
		printf("Could not open shader file %s\n", InFilePath);
		return -1;
	}

	fseek(SFile, 0, SEEK_END);
	srclen = ftell(SFile);
	fseek(SFile, 0, SEEK_SET);

	if (srclen <= 0)
	{
		printf("Error getting shader source size %s\n", InFilePath);
		return -1;
	}

	FileSrc = (char*)malloc(srclen + 1);
	if (FileSrc == 0x0)
	{
		printf("malloc error: shader source\n");
		return -1;
	}
	if ((fread(FileSrc, 1, srclen, SFile)) != srclen)
	{
		printf("read error: shader source\n");
		free(FileSrc);
		FileSrc = 0x0;
		return -1;
	}
	FileSrc[srclen] = '\0';

// Compile shader

	unsigned int Shader;
	Shader = glCreateShader(ShaderType);
	glShaderSource(Shader, 1, &FileSrc, NULL);
	glCompileShader(Shader);

	glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(Shader, 512, NULL, InfoLog);
		printf("GL: Failed to compile shader\n");
		printf("%s\n", InfoLog);
		free(FileSrc);
		FileSrc = 0x0;
		return -1;
	}

	free(FileSrc);
	FileSrc = 0x0;

	return Shader;
}


int shader_t::Create(const char *VertPath, const char *FragPath)
{
	uint32_t VertexShader = Build(VertPath, GL_VERTEX_SHADER);
	if (!VertexShader)
	{
		printf("Could not create shader program\n");
		return -1;
	}
	uint32_t FragmentShader = Build(FragPath, GL_FRAGMENT_SHADER);
	if (!FragmentShader)
	{
		printf("Could not create shader program\n");
		return -1;
	}

	int success;
	ID = glCreateProgram();
	glAttachShader(ID, VertexShader);
	glAttachShader(ID, FragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(ID, SHADER_INFOLOG_SIZE, NULL, InfoLog);
		printf("GL: Failed to link shader program\n");
		printf("%s\n", InfoLog);
		return -1;
	}

// Free memory in GPU after successful program creation

	glDetachShader(ID, VertexShader);
	glDetachShader(ID, FragmentShader);
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return 0;
}


int shader_t::Rebuild()
{
	if (!VertPath || !FragPath)
	{
		printf("Shader Rebuild Error: Cannot call rebuild on an uninitialized shader\n");
		return -1;
	}

	glDeleteProgram(ID);
	Create(VertPath, FragPath);
	return 0;
}


void shader_t::Use()
{
	glUseProgram(ID);
}

#include "shader.h"


int shader_t::Build(const char *InVPath, const char *InFPath)
{
	int len = strlen(InVPath) + 1;
	if(len > TMAX_PATH_LEN)
	{
		printf("Shader Error: vertex file path longer than max\n");
		return -1;
	}
	strcpy(VertPath, InVPath);
	
	len = strlen(InFPath) + 1;
	if(len > TMAX_PATH_LEN)
	{
		printf("Shader Error: fragment file path longer than max\n");
		return -1;
	}
	strcpy(FragPath, InFPath);

	char *VertSrc = 0x0, *FragSrc = 0x0;
	FILE *VertFile = 0x0, *FragFile = 0x0;
	uint64_t srclen = 0;
	int success = 0;

// Read vertex shader

	if( (VertFile = fopen(VertPath, "rb")) == 0x0)
	{
		printf("Could not open vertex file %s\n", VertPath);
		return -1;
	}

	fseek(VertFile, 0, SEEK_END);
	srclen = ftell(VertFile);
	fseek(VertFile, 0, SEEK_SET);

	if(srclen <= 0)
	{
		printf("Error getting vertex source size %s\n", VertPath);
		return -1;
	}

	VertSrc = (char *)malloc(srclen + 1);
	if(VertSrc == 0x0)
	{
		printf("malloc error: vertex source\n");
		return -1;
	}
	if( (fread(VertSrc, 1, srclen, VertFile)) != srclen)
	{
		printf("read error: vertex source\n");
		return -1;
	}
	VertSrc[srclen] = '\0';
	
// Compile vertex shader

	unsigned int VertexShader;
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertSrc, NULL);
	glCompileShader(VertexShader);

	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(VertexShader, 512, NULL, InfoLog);
		printf("GL: Failed to compile vertex shader\n");
		return -1;
	}

	free(VertSrc);
	VertSrc = 0x0;

// Read fragment shader

	if( (FragFile = fopen(FragPath, "rb")) == 0x0)
	{
		printf("Could not open fragment file %s\n", FragPath);
		return -1;
	}
	fseek(FragFile, 0, SEEK_END);
	srclen = ftell(FragFile);
	fseek(FragFile, 0, SEEK_SET);

	if(srclen <= 0)
	{
		printf("Error getting fragment source size %s\n", FragPath);
		return -1;
	}

	FragSrc = (char *)malloc(srclen + 1);
	if(FragSrc == 0x0)
	{
		printf("malloc error: fragment source\n");
		return -1;
	}
	if( (fread(FragSrc, 1, srclen, FragFile)) != srclen)
	{
		printf("read error: fragment source\n");
	}
	FragSrc[srclen] = '\0';

	fclose(FragFile);

// Compile fragment shader

	unsigned int FragmentShader;
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragSrc, NULL);
	glCompileShader(FragmentShader);

	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(FragmentShader, 512, NULL, InfoLog);
		printf("GL: Failed to compile fragment shader\n");
		return -1;
	}

	free(FragSrc);
	FragSrc = 0x0;

// Create shader program

	ID = glCreateProgram();
	glAttachShader(ID, VertexShader);
	glAttachShader(ID, FragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		printf("GL: Failed to link shader program\n");
		return -1;
	}

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return 0;
}


int shader_t::Rebuild()
{
	char *VertSrc = 0x0, *FragSrc = 0x0;
	FILE *VertFile = 0x0, *FragFile = 0x0;
	uint64_t srclen = 0;
	int success = 0;

	glDetachShader(ID, VertexShader);
	glDetachShader(ID, FragmentShader);

// Read vertex shader

	if( (VertFile = fopen(VertPath, "rb")) == 0x0)
	{
		printf("Could not open vertex file %s\n", VertPath);
		return -1;
	}
	fseek(VertFile, 0, SEEK_END);
	srclen = ftell(VertFile);
	fseek(VertFile, 0, SEEK_SET);

	if(srclen <= 0)
	{
		printf("Error getting vertex source size %s\n", VertPath);
		return -1;
	}

	VertSrc = (char *)malloc(srclen + 1);
	if(VertSrc == 0x0)
	{
		printf("malloc error: vertext source\n");
		return -1;
	}
	if( (fread(VertSrc, srclen, 1, VertFile)) != 1)
	{
		printf("read error: vertex source\n");
		return -1;
	}
	VertSrc[srclen] = '\0';

	fclose(VertFile);

// Compile vertex shader

	unsigned int VertexShader;
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertSrc, NULL);
	glCompileShader(VertexShader);

	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		printf("GL: Failed to compile vertex shader\n");
		return -1;
	}

	free(VertSrc);
	VertSrc = 0x0;

// Read fragment shader

	if( (FragFile = fopen(FragPath, "rb")) == 0x0)
	{
		printf("Could not open fragment file %s\n", FragPath);
		return -1;
	}
	fseek(FragFile, 0, SEEK_END);
	srclen = ftell(FragFile);
	fseek(FragFile, 0, SEEK_SET);

	if(srclen <= 0)
	{
		printf("Error getting fragment source size %s\n", FragPath);
		return -1;
	}

	FragSrc = (char *)malloc(srclen + 1);
	if(FragSrc == 0x0)
	{
		printf("malloc error: fragmentt source\n");
		return -1;
	}
	if( (fread(FragSrc, srclen, 1, FragFile)) != 1)
	{
		printf("read error: fragment source\n");
	}
	FragSrc[srclen] = '\0';

	fclose(FragFile);

// Compile fragment shader

	unsigned int FragmentShader;
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragSrc, NULL);
	glCompileShader(FragmentShader);

	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		printf("GL: Failed to compile fragment shader\n");
		return -1;
	}

	free(FragSrc);
	FragSrc = 0x0;

// Create shader program

	glAttachShader(ID, VertexShader);
	glAttachShader(ID, FragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		printf("GL: Failed to link shader program\n");
		return -1;
	}

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return 0;
}


void shader_t::Use()
{
	glUseProgram(ID);
}


int shader_t::SetUniform(const char *Uniform, int Type, void *Data)
{
	int UniformLocation = glGetUniformLocation(ID, Uniform);
	if(UniformLocation == -1)
	{
		printf("Shader Error: could not locate uniform %s\n", Uniform);
		return -1;
	}

	glUseProgram(ID);

	switch (Type)
	{
		case VEC3_F:
		{
			uMATH::vec3f_t *Input = (uMATH::vec3f_t *)Data; 
			glUniform3f(UniformLocation, Input->x, Input->y, Input->z);
			break;
		}
		case VEC4_F:
		{
			uMATH::vec4f_t *Input = (uMATH::vec4f_t *)Data; 
			glUniform4f(UniformLocation, Input->x, Input->y, Input->z, Input->w);
			break;
		}
		default:
		{
			printf("Shader Error :: Set Uniform: no matching type\n");
			return -1;
		}
	}

	return 0;
}

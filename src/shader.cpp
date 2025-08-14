#include "shader.h"


int shader_info_t::Init(const char *V,const char *TC,const char *TE,const char *G,const char *F,const char *C)
{
	int res = 0;

	if (V)
	{
		res = AddFile(V, 0);
		if (res != 0) return -1; 
		PipelineOpts |= VOID_VERT_OPT;
	}
	if (TC)
	{
		res = AddFile(TC, 1);
		if (res != 0) return -1; 
		PipelineOpts |= VOID_TESCC_OPT;
	}
	if (TE)
	{
		res = AddFile(TE, 2);
		if (res != 0) return -1; 
		PipelineOpts |= VOID_TESCE_OPT;
	}
	if (G)
	{
		res = AddFile(G, 3);
		if (res != 0) return -1; 
		PipelineOpts |= VOID_GEOM_OPT;
	}
	if (F)
	{
		res = AddFile(F, 4);
		if (res != 0) return -1; 
		PipelineOpts |= VOID_FRAG_OPT;
	}
	if (C)
	{
		res = AddFile(C, 5);
		if (res != 0) return -1; 
		PipelineOpts |= VOID_COMP_OPT;
	}

	return 0;
}


int shader_info_t::AddFile(const char *InFile, int FileType)
{
	size_t len = strlen(InFile) + 1;
	if (len > TMAX_PATH_LEN)
	{
		printf("Shader %s: file path longer than max\n", InFile);
		return -1;
	}
	memcpy(&FilePaths[FileType][0], InFile, len); 

	return 0;
}


uint32_t shader_program_t::Build(const char* InFilePath, int ShaderType)
{
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
		fclose(SFile);
		return -1;
	}

	FileSrc = (char*)malloc(srclen + 1);
	if (FileSrc == 0x0)
	{
		printf("malloc error: shader source\n");
		fclose(SFile);
		return -1;
	}
	if ((fread(FileSrc, 1, srclen, SFile)) != srclen)
	{
		printf("read error: shader source %s\n", InFilePath);
		free(FileSrc);
		FileSrc = 0x0;
		fclose(SFile);
		return -1;
	}
	FileSrc[srclen] = '\0';
	fclose(SFile);

	// Compile shader

	unsigned int Shader;
	Shader = glCreateShader(ShaderType);
	glShaderSource(Shader, 1, &FileSrc, NULL);
	glCompileShader(Shader);

	glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(Shader, 512, NULL, InfoLog);
		printf("GL: Failed to compile shader %s\n", InFilePath);
		printf("%s\n", InfoLog);
		free(FileSrc);
		FileSrc = 0x0;
		return -1;
	}

	free(FileSrc);
	FileSrc = 0x0;

	return Shader;
}


int shader_program_t::Create(const shader_info_t &inParams)
{
	memcpy(&Params, &inParams, sizeof(shader_info_t));

	uint32_t Shaders[6] = {};

	char *FilePath = 0x0;

	// VERT
	if (Params.PipelineOpts & VOID_VERT_OPT)
	{
		FilePath = &Params.FilePaths[0][0];
		Shaders[0] = Build(FilePath, GL_VERTEX_SHADER);
		if (!Shaders[0])
		{
			printf("SHADER: Could not create shader program\n");
			return -1;
		}
	}
	// TESC CN
	if (Params.PipelineOpts & VOID_TESCC_OPT)
	{
		if( !(Params.PipelineOpts & VOID_TESCE_OPT) )
		{
			printf("SHADER: No Tesselation Evaluation shader supplied\n");
			printf("Could not create shader program\n");
			return -1;
		}

		FilePath = &Params.FilePaths[1][0];
		Shaders[1] = Build(FilePath, GL_TESS_CONTROL_SHADER);
		if (!Shaders[1])
		{
			printf("SHADER: Could not create shader program\n");
			return -1;
		}
	}
	// TESC EV
	if (Params.PipelineOpts & VOID_TESCE_OPT)
	{
		if( !(Params.PipelineOpts & VOID_TESCC_OPT) )
		{
			printf("SHADER: No Tesselation Control shader supplied\n");
			printf("Could not create shader program\n");
			return -1;
		}

		FilePath = &Params.FilePaths[2][0];
		Shaders[2] = Build(FilePath, GL_TESS_EVALUATION_SHADER);
		if (!Shaders[2])
		{
			printf("SHADER: Could not create shader program\n");
			return -1;
		}
	}
	// GEOM
	if (Params.PipelineOpts & VOID_GEOM_OPT)
	{
		FilePath = &Params.FilePaths[3][0];
		Shaders[3] = Build(FilePath, GL_GEOMETRY_SHADER);
		if (!Shaders[3])
		{
			printf("Could not create shader program\n");
			return -1;
		}
	}
	// FRAG
	if (Params.PipelineOpts & VOID_FRAG_OPT)
	{
		FilePath = &Params.FilePaths[4][0];
		Shaders[4] = Build(FilePath, GL_FRAGMENT_SHADER);
		if (!Shaders[4])
		{
			printf("Could not create shader program\n");
			return -1;
		}
	}
	// COMP
	if (Params.PipelineOpts & VOID_COMP_OPT)
	{
		if(Params.PipelineOpts != VOID_COMP_OPT)
		{
			printf("SHADER: Compute shaders must be standalone\n");
			printf("Could not create shader program\n");
			return -1;
		}

		FilePath = &Params.FilePaths[5][0];
		Shaders[5] = Build(FilePath, GL_COMPUTE_SHADER);
		if (!Shaders[5])
		{
			printf("Could not create shader program\n");
			return -1;
		}
	}

	int success;
	ID = glCreateProgram();

	for(int i = 0; i < 6; i++)
	{
		if (Shaders[i])
		{
			glAttachShader(ID, Shaders[i]);
		}
	}

	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(ID, SHADER_INFOLOG_SIZE, NULL, InfoLog);
		printf("GL: Failed to link shader program\n");
		printf("%s\n", InfoLog);
		return -1;
	}

// Storing info in GPU memory unnecessary after successful program creation - free it

	for(int i = 0; i < 6; i++)
	{
		if (Shaders[i])
		{
			glDetachShader(ID, Shaders[i]);
			glDeleteShader(Shaders[i]);
		}
	}

	return 0;
}


int shader_program_t::Rebuild()
{
	if (!Params.PipelineOpts)
	{
		printf("Shader Rebuild Error: Cannot call rebuild on an uninitialized shader\n");
		return -1;
	}

	glDeleteProgram(ID);
	Create(Params);
	return 0;
}


void shader_program_t::Use()
{
	glUseProgram(ID);
}

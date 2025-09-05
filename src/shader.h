#ifndef VOID_SHADERS_H
#define VOID_SHADERS_H


#include "../vendor/glad/glad.h"
#include "pal/PAL.h"
#include <stdint.h>
#include <stdlib.h>


#define SHADER_INFOLOG_SIZE 512
#define VOID_VERT_OPT 0b1 
#define VOID_TESCC_OPT 0b10 
#define VOID_TESCE_OPT 0b100 
#define VOID_GEOM_OPT 0b1000 
#define VOID_FRAG_OPT 0b10000
#define VOID_COMP_OPT 0b100000 


struct shader_info_t
{
	char PipelineOpts : 6;
	char : 0;
	char FilePaths[6][VOID_PATH_MAX];

	int Init(const char *V,const char *TC,const char *TE,const char *G,const char *F,const char *C);

	private:
	int AddFile(const char *InFile, int FileType);
};


struct shader_program_t
{
	uint32_t ID;
	char InfoLog[SHADER_INFOLOG_SIZE];
	shader_info_t Params;

	int Create(const shader_info_t &inParams);
	int Rebuild();
	void Use();

	private:
	uint32_t Build(const char *FilePath, int ShaderType);
};


#endif


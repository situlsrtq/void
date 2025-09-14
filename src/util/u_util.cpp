#include "u_util.h"


namespace UTIL
{


int fopen_GetFileSize(const char* InFilePath, FILE*& File, size_t* SrcLen)
{
	File = 0x0;

	if ((File = fopen(InFilePath, "rb")) == 0x0)
	{
		printf("Could not open shader file %s\n", InFilePath);
		return EXIT_FAILURE;
	}

	fseek(File, 0, SEEK_END);
	*SrcLen = ftell(File);
	fseek(File, 0, SEEK_SET);

	if (*SrcLen <= 0)
	{
		printf("Error getting shader source size %s\n", InFilePath);
		fclose(File);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


int GetFileSize(const char* InFilePath, size_t* SrcLen)
{
	FILE* File = 0x0;

	if ((File = fopen(InFilePath, "rb")) == 0x0)
	{
		printf("Could not open shader file %s\n", InFilePath);
		return EXIT_FAILURE;
	}

	fseek(File, 0, SEEK_END);
	*SrcLen = ftell(File);
	fseek(File, 0, SEEK_SET);

	if (*SrcLen <= 0)
	{
		printf("Error getting shader source size %s\n", InFilePath);
		fclose(File);
		return EXIT_FAILURE;
	}

	fclose(File);
	return EXIT_SUCCESS;
}


}

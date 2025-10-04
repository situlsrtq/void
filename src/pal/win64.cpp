#include "PAL.h"
#ifdef VOID_PLATFORM_WIN64

#include <windows.h>

#define VOID_DIR_HEAD "\\void\\"

char g_PathBuffer_r[VOID_PATH_MAX + 1];
char* g_OSPath_r = g_PathBuffer_r;

int PAL::GetPath(char* buf, int64_t size)
{
	printf("-----WARNING, OBTAINIG EXE PATH AT RUNTIME-----\n");
	printf("If you are actively working on source code this is fine.\n");
	printf("If you are an end user, you have a debug software version. Get an official release\n");
	printf("------------------------------------------------\n");

	DWORD len = GetModuleFileName(NULL, buf, VOID_PATH_MAX);
	if(len >= size)
	{
		printf("GetPath: truncation would occur. Retry with a larger buffer\n");
		return VOID_BAD_BUFFER_SIZE;
	}
	if(len == 0)
	{
		DWORD err = GetLastError();
		printf("PAL Error: GetFile() Err: %d\n", err);
		return EXIT_FAILURE;
	}

	// GetModuleFileName() gives us the full path, including the executable, but we only want the base directory
	// for the program, so we use a sliding window to find it and trim the unnecessary data

	int i = len, k = len - 1, res = 1;
	char temp = '/';
	char* slide;
	while(res)
	{
		if(i <= 1)
		{
			printf("GetPath: path not found. Check configuration\n");
			return EXIT_FAILURE;
		}

		i = k + 1;
		k--;

		while(buf[k] != '\\')
		{
			k--;
		}

		temp = buf[i];
		buf[i] = '\0';

		slide = &buf[k];
		res = strcmp(slide, VOID_DIR_HEAD);
		buf[i] = temp;
	}

	buf[i] = '\0';

	return EXIT_SUCCESS;
}

void* PAL::AlignedAlloc(size_t size, size_t alignment)
{
	return _aligned_malloc(size, alignment);
}

#endif

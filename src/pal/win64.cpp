#include "PAL.h"
#ifdef VOID_PLATFORM_WIN64


#include <windows.h>


int PAL::GetPath(char *buf, size_t size)
{
	DWORD len = GetModuleFileName(NULL, path, VOID_PATH_MAX);
	if(len >= size)
	{
		printf("GetPath: truncation would occur. Retry with a larger buffer\n");
		return VOID_BAD_BUFFER_SIZE;
	}
	if(len == 0)
 	{
		DWORD errno = GetLastError();
		printf("PAL Error: GetFile() Err: %d" errno);
		return EXIT_FAILURE;
	}

	buf[len] = '\0';
	return EXIT_SUCCESS;
}


#endif

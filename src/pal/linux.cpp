#include "PAL.h"
#ifdef VOID_PLATFORM_LINUX


#include <unistd.h>


int PAL::GetPath(char *buf, size_t size)
{
	printf("-----WARNING, OBTAINIG EXE PATH AT RUNTIME-----\n");
	printf("If you are actively working on source code this is fine.\n");
	printf("If you are an end user, you have an unofficial and unsafe software version. Get an official release \n");
	printf("------------------------------------------------\n");

	size_t len = readlink("/proc/self/exe", buf, size);
	if(len >= size)
	{
		printf("GetPath: truncation would occur. Retry with a larger buffer\n");
		return VOID_BAD_BUFFER_SIZE;
	}
	if(len == -1)
	{
		perror("PAL: GetFile():");
		return EXIT_FAILURE;
	}

	// readlink() gives us the full path, including the executable, but we only want the base directory
	// for the program, so we use a sliding window to find it and trim the unnecessary data
	
	int i = 0, k = 1, res = 1;
	char temp = '/';
	char* slide;
	while(res)
	{
		slide = &buf[i];
		while(buf[k] != '/')
		{
			k++;
		}

		k++;

		temp = buf[k];
		buf[k] = '\0';

		res = strcmp(slide, "/void/");
		buf[k] = temp;
		i = k-1;
	}

	buf[k] = '\0';

	return EXIT_SUCCESS;
}


#endif

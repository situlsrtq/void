#ifndef VOID_PAL_H
#define VOID_PAL_H

// TODO: Get rid of runtime path discovery (MAX_PATH/PATH_MAX/etc) in release builds

#ifdef _WIN64
#define VOID_PLATFORM_WIN64
#define VOID_PATH_MAX MAX_PATH
#elif __linux__
#define VOID_PLATFORM_LINUX
#define VOID_PATH_MAX PATH_MAX
#endif


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
// NOTE: limits is only here because of the runtime path shit
#include <limits.h>


#define VOID_BAD_BUFFER_SIZE -1000

#define SCREEN_X_DIM_DEFAULT 1000.0f
#define SCREEN_Y_DIM_DEFAULT 800.0f


namespace PAL
{


#ifdef DEBUG
int GetPath(char* buf, size_t size);
#endif

void* AlignedAlloc(size_t size, size_t alignment);


}


#endif

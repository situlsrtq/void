#ifndef VOID_PAL_H
#define VOID_PAL_H

// TODO: Get rid of runtime path discovery (MAX_PATH/PATH_MAX/etc) in release builds

#ifdef _WIN64
#define VOID_PLATFORM_WIN64
#elif __linux__
#define VOID_PLATFORM_LINUX
#endif

#define VOID_PATH_MAX 256

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
// NOTE: limits is only here because of the runtime path shit
#include <limits.h>

#define VOID_BAD_BUFFER_SIZE -1000

#define SCREEN_X_DIM_DEFAULT 1000.0f
#define SCREEN_Y_DIM_DEFAULT 1000.0f

//------- TODO: Move to Core module when such a thing exists ----------
#ifdef DEBUG
extern char g_PathBuffer_r[VOID_PATH_MAX + 1];
extern char* g_OSPath_r;
#endif

#define V_KIB (1024ULL)
#define V_MIB (1024ULL * V_KIB)
#define V_GIB (1024ULL * V_MIB)
#define V_TIB (1024ULL * V_GIB)
//--------------------------------------------------------------------

namespace PAL
{

#ifdef DEBUG
int GetPath(char* buf, int64_t size);
#endif

void* AlignedAlloc(size_t size, size_t alignment);

} // namespace PAL

#endif

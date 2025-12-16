#ifndef VOID_PAL_H
#define VOID_PAL_H

// TODO: Get rid of runtime path discovery (MAX_PATH/PATH_MAX/etc) in release builds

#ifdef _WIN64
#define VOID_PLATFORM_WIN64
#elif __linux__
#define VOID_PLATFORM_LINUX
#endif

#include "u_types.h"
// NOTE: limits is only here because of the runtime path shit
#include <limits.h>

namespace PAL
{

#ifdef DEBUG
int GetPath(char* buf, int64_t size);
#endif

void* AlignedAlloc(size_t size, size_t alignment);

} // namespace PAL

#endif

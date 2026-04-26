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
int get_path(char* buf, int64_t size);
#endif

void* map_mem_page(size_t size);
void free_pages(void* addr, size_t size);
void* aligned_alloc(size_t size, size_t alignment);

} // namespace PAL

#endif

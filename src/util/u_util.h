#ifndef VOID_UTIL_H
#define VOID_UTIL_H

#include <cstddef>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

namespace UTIL
{

int fopen_GetFileSize(const char* InFilePath, FILE*& File, size_t* SrcLen);

int GetFileSize(const char* InFilePath, size_t* SrcLen);

inline void* Malloc(size_t size)
{
	void* res = malloc(size);
	assert(res != 0x0);
	return res;
}

template<typename T>
inline void Free(T*& ptr)
{
	free(ptr);
	ptr = 0x0;
}

} // namespace UTIL

#endif

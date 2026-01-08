#ifndef VOID_TYPES_H
#define VOID_TYPES_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#define PROGRAM_MAX_OBJECTS 512
#define OBJECT_ALLOC_ERROR PROGRAM_MAX_OBJECTS + 1

#define VOID_PATH_MAX 256
#define VOID_BAD_BUFFER_SIZE -1000

#define SCREEN_X_DIM_DEFAULT 1000.0f
#define SCREEN_Y_DIM_DEFAULT 1000.0f

#define KIB (1024ULL)
#define MIB (1024ULL * KIB)
#define GIB (1024ULL * MIB)
#define TIB (1024ULL * GIB)

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

inline constexpr u64 V_KIB(u32 IN)
{
	return (u64)(IN * KIB);
}
inline constexpr u64 V_MIB(u32 IN)
{
	return (u64)(IN * MIB);
}
inline constexpr u64 V_GIB(u32 IN)
{
	return (u64)(IN * GIB);
}
inline constexpr u64 V_TIB(u32 IN)
{
	return (u64)(IN * TIB);
}

//------- TODO: Move to Core module when such a thing exists ----------
#ifdef DEBUG
extern char g_PathBuffer_r[VOID_PATH_MAX + 1];
extern char* g_OSPath_r;
#endif
//--------------------------------------------------------------------

#endif

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

#define V_KIB (1024ULL)
#define V_MIB (1024ULL * V_KIB)
#define V_GIB (1024ULL * V_MIB)
#define V_TIB (1024ULL * V_GIB)

//------- TODO: Move to Core module when such a thing exists ----------
#ifdef DEBUG
extern char g_PathBuffer_r[VOID_PATH_MAX + 1];
extern char* g_OSPath_r;
#endif
//--------------------------------------------------------------------

#endif
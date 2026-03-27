#ifndef VOID_RTYPES_H
#define VOID_RTYPES_H

#include "u_types.h"

#define VOID_TEX_COUNT 3
#define VOID_VBUFCOUNT_FMT 5
#define VOID_TEXCOUNT_FMT 3
#define INDEX_BUFFER 0
#define POS_BUFFER 1
#define NORM_BUFFER 2
#define TAN_BUFFER 3
#define TEX_BUFFER 4

struct vertex_buffer_info_t
{
	u32 vbuffer_array[VOID_VBUFCOUNT_FMT];
	u64 index_byte_offset;
	u64 pos_offset;
	u64 norm_offset;
	u64 tan_offset;
	u64 tex_offset;
};

struct index_info_t
{
	u64 index_count;
	u64 ebo_byte_offset;
	u32 index_type;
};

struct vertex_info_t
{
	u64 vattr_count;
	u64 vertex_offset;
};

struct texture_info_t
{
	u32 tex_count;
	u32 tex_array[VOID_TEXCOUNT_FMT];
};

#endif

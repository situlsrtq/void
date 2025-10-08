#ifndef VOID_RTYPES_H
#define VOID_RTYPES_H

#include <stdint.h>

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
	uint32_t VBufferArray[VOID_VBUFCOUNT_FMT];
	uint64_t CurrIndexByteOffset;
	uint64_t CurrPosOffset;
	uint64_t CurrNormOffset;
	uint64_t CurrTanOffset;
	uint64_t CurrTexOffset;
};

struct index_info_t
{
	uint64_t IndexCount;
	uint64_t ByteOffsetEBO;
	uint32_t IndexType;
};

struct vertex_info_t
{
	uint64_t VAttrCount;
	uint64_t VertexOffset;
};

struct texture_info_t
{
	unsigned int TexCount;
	unsigned int TexArray[VOID_TEXCOUNT_FMT];
};

#endif

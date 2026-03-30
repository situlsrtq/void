#ifndef VOID_RTYPES_H
#define VOID_RTYPES_H

#include "u_mem.h"
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

struct draw_command_info_t
{
	u32 node_id;
};

struct command_buffer_t
{
	draw_command_info_t* command_list;
	u32 max_command_count;
	u32 curr_command_count;
};

///< summary>
/// Opens a command list for writing
///</summary>
inline int command_buffer_frame_start(linear_arena_t* scratch_arena, command_buffer_t* buffer, u32 max_count)
{
	u64 handle;
	int res = arena_alloc(scratch_arena, &handle, max_count * sizeof(draw_command_info_t));
	if(res == EXIT_FAILURE)
	{
		printf("System: could not allocate command buffer\n");
		return EXIT_FAILURE;
	}

	buffer->command_list = (draw_command_info_t*)pointer_from_arena(scratch_arena, handle);
	buffer->max_command_count = max_count;
	buffer->curr_command_count = 0;

	return EXIT_SUCCESS;
}

inline void command_buffer_add_command(command_buffer_t* buffer, draw_command_info_t command)
{
	if(buffer->curr_command_count == buffer->max_command_count)
	{
		printf("Command buffer: call not added - command buffer full\n");
		return;
	}

	buffer->command_list[buffer->curr_command_count] = command;
	buffer->curr_command_count++;
}

///< summary>
/// Closes a command list. Assumes scratch space arena, which will be cleared at frame end
///</summary>
inline void command_buffer_frame_end(command_buffer_t* buffer)
{
	buffer->command_list = 0x0;
}

#endif

#ifndef VOID_RTYPES_H
#define VOID_RTYPES_H

#include "tracy_wrapper.h"
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

#define CMD_BUF_FULL -5

struct vertex_buffer_info_t
{
	u32 vbuffer_array[VOID_VBUFCOUNT_FMT];
	u64 offsets[5];
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
	u32* node_bitfield;
	u32 max_command_count;
	u32 curr_command_count;
};

///< summary>
/// Opens a command list for writing
///</summary>
inline int command_buffer_frame_start(linear_arena_t* scratch_arena, command_buffer_t* buffer, u32 max_count)
{
	size_t handle;
	int res = arena_alloc(scratch_arena, &handle, max_count * sizeof(draw_command_info_t));
	if(res == EXIT_FAILURE)
	{
		printf("System: could not allocate command buffer\n");
		return EXIT_FAILURE;
	}

	buffer->command_list = (draw_command_info_t*)pointer_from_arena(scratch_arena, handle);

	res = arena_alloc(scratch_arena, &handle, bitfield_min_size32(max_count));
	if(res == EXIT_FAILURE)
	{
		printf("System: could not allocate command buffer\n");
		return EXIT_FAILURE;
	}

	buffer->node_bitfield = (u32*)pointer_from_arena(scratch_arena, handle);

	memset(buffer->node_bitfield, 0, bitfield_min_size32(max_count)); // the scratch arena will likely contain garbage data where this array gets allocated
	buffer->max_command_count = max_count;
	buffer->curr_command_count = 0;

	return EXIT_SUCCESS;
}

inline int command_buffer_add_command(command_buffer_t* buffer, draw_command_info_t command)
{
	if(buffer->curr_command_count == buffer->max_command_count)
	{
		printf("Command buffer: call not added - command buffer full\n");
		return CMD_BUF_FULL;
	}

	if(bitfield_is_set32(buffer->node_bitfield, command.node_id))
	{
		// Duplicate node entry attempted
		return EXIT_SUCCESS;	
	}

	buffer->command_list[buffer->curr_command_count] = command;
	buffer->curr_command_count++;
	bitfield_set32(buffer->node_bitfield, command.node_id);
	return EXIT_SUCCESS;
}

inline int compare_commands(const void* c1, const void* c2)
{
	draw_command_info_t* dc1 = (draw_command_info_t*)c1;
	draw_command_info_t* dc2 = (draw_command_info_t*)c2;

	if(dc1->node_id < dc2->node_id) return -1;
	if(dc1->node_id == dc2->node_id) return 0;
	else return 1;
}

///< summary>
/// Closes a command list. Should always be called before making any draw calls out of the buffer to prevent duplicate draws
///</summary>
inline void command_buffer_frame_end(command_buffer_t* buffer)
{
	ZONE_SCOPED;
	if(buffer->curr_command_count <= 1)
	{
		return;
	}
	qsort(buffer->command_list, buffer->curr_command_count, sizeof(draw_command_info_t), compare_commands);
}

#endif

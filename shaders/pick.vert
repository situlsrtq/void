#version 460 core

layout(std430, binding = 0) buffer vertex_ssbo
{
        float vert_data[];
};

uniform uint pos_offset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
        uint pos_idx = pos_offset + (gl_VertexID * 3U);
        vec3 aPos = vec3(vert_data[pos_idx], vert_data[pos_idx + 1U], vert_data[pos_idx + 2U]);
        gl_Position = projection * view * model * vec4(aPos, 1.0);
}

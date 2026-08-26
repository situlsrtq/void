#version 460 core

layout(std430, binding = 0) readonly buffer vertex_ssbo
{
        float vert_data[];
};

vec2 vertex_pull_2f(uint index)
{
        return vec2(vert_data[index], vert_data[index + 1U]);
}
vec3 vertex_pull_3f(uint index)
{
        return vec3(vert_data[index], vert_data[index + 1U], vert_data[index + 2U]);
}
vec4 vertex_pull_4f(uint index)
{
        return vec4(vert_data[index], vert_data[index + 1U], vert_data[index + 2U], vert_data[index + 3U]);
}

uniform uint pos_offset;
uniform uint norm_offset;
uniform uint tan_offset;
uniform uint tex_offset;

uniform mat3 minvt;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 WorldPos;
out vec3 Normal;
out vec4 Tangent;
out vec2 TexCoord;

void main()
{
        vec3 aPos = vertex_pull_3f(pos_offset + (gl_VertexID * 3U));
        vec3 aNorm = vertex_pull_3f(norm_offset + (gl_VertexID * 3U));
        vec4 aTan = vertex_pull_4f(tan_offset + (gl_VertexID * 4U));
        vec2 aTex = vertex_pull_2f(tex_offset + (gl_VertexID * 2U));

        gl_Position = projection * view * model * vec4(aPos, 1.0);
        WorldPos = vec3(model * vec4(aPos, 1.0));
        Normal = normalize(minvt * aNorm);
        Tangent = aTan;
        TexCoord = aTex;
}

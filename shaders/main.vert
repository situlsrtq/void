#version 460 core

layout(std430, binding = 0) buffer vertex_ssbo
{
        float vert_data[];
};

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
        uint pos_idx = pos_offset + (gl_VertexID * 3U);
        vec3 aPos = vec3(vert_data[pos_idx], vert_data[pos_idx + 1U], vert_data[pos_idx + 2U]);

        uint norm_idx = norm_offset + (gl_VertexID * 3U);
        vec3 aNorm = vec3(vert_data[norm_idx], vert_data[norm_idx + 1U], vert_data[norm_idx + 2U]);

        uint tan_idx = tan_offset + (gl_VertexID * 4U);
        vec4 aTan = vec4(vert_data[tan_idx], vert_data[tan_idx + 1U], vert_data[tan_idx + 2U], vert_data[tan_idx + 3U]);

        uint tex_idx = tex_offset + (gl_VertexID * 2U);
        vec2 aTex = vec2(vert_data[tex_idx], vert_data[tex_idx + 1U]);

        gl_Position = projection * view * model * vec4(aPos, 1.0);
        WorldPos = vec3(model * vec4(aPos, 1.0));
        Normal = normalize(minvt * aNorm);
        Tangent = aTan;
        TexCoord = aTex;
}

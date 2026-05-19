#version 460 core

out vec2 TexCoord;

vec3 positions[3] = vec3[](vec3(-1.0, -1.0, 0.0), vec3(3.0, -1.0, 0.0), vec3(-1.0, 3.0, 0.0));
vec2 texcoords[3] = vec2[](vec2(0.0, 0.0), vec2(2.0, 0.0), vec2(0.0, 2.0));

void main()
{
        gl_Position = vec4(positions[gl_VertexID], 1.0);
        TexCoord = texcoords[gl_VertexID];
}

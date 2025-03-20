#version 460 core

out vec2 FragColor;

uniform float index;
uniform float type;

void main()
{
	FragColor = vec2(index, type);
}

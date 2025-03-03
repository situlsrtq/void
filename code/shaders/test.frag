#version 460 core

in vec3 Normal;
in vec3 WorldPos;

out vec4 FragColor;

uniform vec3 lightpos;
uniform vec3 objcolor;
uniform vec3 lightcolor;
uniform float ambientstrength;

void main()

{
// ambient
	vec3 ambient = ambientstrength * lightcolor;

// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightdir = normalize(lightpos - WorldPos);
	float angle = max(dot(norm, lightdir), 0.0);
	vec3 diffuse = angle * lightcolor;

// output
	vec3 result = objcolor * (ambient + diffuse);
	FragColor = vec4(result, 1.0f);
}

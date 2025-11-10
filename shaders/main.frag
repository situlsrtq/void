#version 460 core

in vec3 WorldPos;
in vec3 Normal;
in vec4 Tangent;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightpos;
uniform vec3 objcolor;
uniform vec3 lightcolor;
uniform vec3 viewpos;
uniform float ambientstrength;

layout(binding = 0) uniform sampler2D alb_map;
layout(binding = 1) uniform sampler2D metlrgh_map;
layout(binding = 2) uniform sampler2D norm_map;

void main()

{
    vec4 Objcolor = texture(alb_map, TexCoord);

    // bitangent frame perturbed normals
    vec3 vNt = texture(norm_map, TexCoord).rgb * 2.0f - 1.0f;
    vec3 tangent = Tangent.xyz;
    vec3 bitangent = cross(Normal, Tangent.xyz) * Tangent.w;
    vec3 norm = normalize(vNt.x * tangent + vNt.y * bitangent + vNt.z * Normal);

    // ambient
    vec3 ambient = ambientstrength * lightcolor;

    // diffuse
    vec3 lightdir = normalize(lightpos - WorldPos);
    float angle = max(dot(norm, lightdir), 0.0);
    vec3 diffuse = angle * lightcolor;

    // specular
    float specularstrength = 0.5;
    vec3 viewdir = normalize(viewpos - WorldPos);
    vec3 reflectdir = reflect(-lightdir, norm);
    float spec = pow(max(dot(viewdir, reflectdir), 0.0), 32);
    vec3 specular = specularstrength * spec * lightcolor;

    // output
    FragColor = Objcolor * (vec4(ambient, 1.0f) + vec4(diffuse, 1.0f) + vec4(specular, 1.0f));
}

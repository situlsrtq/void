#version 460 core

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightpos;
uniform vec3 objcolor;
uniform vec3 lightcolor;
uniform vec3 viewpos;
uniform float ambientstrength;

uniform sampler2D albedo;

void main()

{
    vec4 Objcolor = texture(albedo, TexCoord);
    vec3 o = vec3(Objcolor);

    // ambient
    vec3 ambient = ambientstrength * lightcolor;

    // diffuse
    vec3 norm = normalize(Normal);
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
    vec3 result = o * (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);
}

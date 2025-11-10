#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform float exposure;
uniform sampler2D HDRtex;

void main()
{
    const float gamma = 2.2;
    vec3 HDRcol = texture(HDRtex, TexCoord).rgb;

    vec3 tonemapped = vec3(1.0) - exp(-HDRcol * exposure);
    tonemapped = pow(tonemapped, vec3(1.0 / gamma));

    FragColor = vec4(tonemapped, 1.0);
}

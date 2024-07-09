#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
layout(location = 0) uniform sampler2D texture1;
layout(location = 1) uniform sampler2D texture2;

void main()
{
    FragColor = vec4(1);
}
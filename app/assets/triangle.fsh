#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
layout(location = 0) uniform sampler2D texture1;
layout(location = 1) uniform sampler2D texture2;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.9);
}
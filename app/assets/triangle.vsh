#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

//layout(location = 2) uniform mat4 model;
//layout(location = 3) uniform mat4 view;
//layout(location = 4) uniform mat4 projection;

void main()
{
    // fill in the TexCoord variable
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
//    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}
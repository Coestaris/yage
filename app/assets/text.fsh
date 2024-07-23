#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform vec4 color;
uniform sampler2D atlas;

void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, texture(atlas, TexCoords).r) * color;
    FragColor = vec4(1.0, 1.0, 1.0, 1.0) * color * texture(atlas, TexCoords).r;
    FragColor += vec4(1.0, 1.0, 1.0, 1.0);
}
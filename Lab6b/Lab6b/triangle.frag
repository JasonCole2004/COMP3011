#version 450 core
layout (location = 0) out vec4 fColour;

in vec2 tex;

uniform sampler2D Texture;

void main()
{
    fColour = texture(Texture, tex);
}
#version 450 core

in vec3 colour;
out vec4 fColour;

void main()
{
    fColour = vec4(colour, 1.0);
}

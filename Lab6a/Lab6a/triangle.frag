#version 450 core

in vec3 col;
out vec4 fColour;

void main()
{
    fColour = vec4(col, 1.0);
}
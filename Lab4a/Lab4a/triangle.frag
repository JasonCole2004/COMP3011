#version 450 core

uniform vec3 uCol;
out vec4 fColour;

void main()
{
    fColour = vec4(uCol, 1.0);
}

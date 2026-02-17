#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vCol;

uniform mat4 transformation;
out vec3 colour;

void main()
{
    gl_Position = transformation * vPos;
    colour = vCol;
}

#version 450 core

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec3 vCol;

out vec3 colour;
uniform float x_offset; 
uniform float y_offset;

void main()
{
    gl_Position = vec4(vPos.x+x_offset, vPos.y+y_offset, 0.f, 1.f);
    colour = vCol;
}

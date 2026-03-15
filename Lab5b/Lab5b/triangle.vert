#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vCol;

out vec4 col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vPos;
	col = vCol;
}

#version 450 core

layout (location = 0) out vec4 fColour;

in vec4 col;


void main()
{
	fColour = col;
}

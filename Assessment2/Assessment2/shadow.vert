#version 450 core

// Minimal vertex shader for the shadow pass: only position needed
layout(location = 0) in vec3 vPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(vPos, 1.0);
}

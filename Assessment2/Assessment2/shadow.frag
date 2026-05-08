#version 450 core

in vec3 fragWorldPos;

uniform vec3  lightPos;
uniform float farPlane;

void main()
{
    // Store linear distance from the light rather than projected depth,
    // so the cube map can be sampled with a direction vector in the main pass.
    gl_FragDepth = length(fragWorldPos - lightPos) / farPlane;
}

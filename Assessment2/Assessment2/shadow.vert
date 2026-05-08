#version 450 core

layout(location = 0) in vec3 vPos;

uniform mat4 model;
uniform mat4 shadowMatrix;

out vec3 fragWorldPos;

void main()
{
    vec4 worldPos = model * vec4(vPos, 1.0);
    fragWorldPos  = worldPos.xyz;
    gl_Position   = shadowMatrix * worldPos;
}

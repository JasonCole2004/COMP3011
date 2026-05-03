#version 450 core

// Each lens flare sprite is positioned in NDC space via the model matrix
layout(location = 0) in vec3 vPos;
layout(location = 3) in vec2 vUV;

uniform mat4 model;

out vec2 texCoord;

void main()
{
    gl_Position = model * vec4(vPos, 1.0);
    texCoord    = vUV;
}

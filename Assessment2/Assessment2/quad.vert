#version 450 core

// Full-screen quad pass-through used by all post-processing shaders
layout(location = 0) in vec3 vPos;
layout(location = 3) in vec2 vUV;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(vPos.xy, 0.0, 1.0);
    texCoord    = vUV;
}

#version 450 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vCol;
layout(location = 2) in vec3 vNor;
layout(location = 3) in vec2 vUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 col;
out vec3 nor;
out vec3 FragPos;
out vec2 texCoord;

void main()
{
    vec4 worldPos = model * vec4(vPos, 1.0);
    gl_Position   = projection * view * worldPos;

    col      = vCol;
    FragPos  = vec3(worldPos);
    texCoord = vUV;

    // Transform normal into world space using the normal matrix
    nor = normalize(mat3(transpose(inverse(model))) * vNor);
}

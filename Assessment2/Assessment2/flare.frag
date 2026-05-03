#version 450 core

in vec2 texCoord;

uniform vec4 flareColour;

out vec4 fColour;

void main()
{
    // Map UVs to -1..1 and compute distance from sprite centre
    vec2  uv   = texCoord * 2.0 - 1.0;
    float dist = length(uv);

    // Quadratic falloff so the edge fades smoothly to transparent
    float alpha = max(0.0, 1.0 - dist);
    alpha = alpha * alpha;

    fColour = vec4(flareColour.rgb, flareColour.a * alpha);
}

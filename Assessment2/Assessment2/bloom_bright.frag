#version 450 core

in vec2 texCoord;

// Only the sun writes to this texture (via MRT in triangle.frag)
// so no threshold is needed: everything here should glow
uniform sampler2D emissive;

out vec4 fColour;

void main()
{
    vec3  col       = texture(emissive, texCoord).rgb;
    float luminance = dot(col, vec3(0.2126, 0.7152, 0.0722));

    // Blend toward a warm white so the corona looks like scattered light
    // rather than just a blurry orange blob
    col     = mix(col, vec3(1.0, 0.97, 0.88) * luminance, 0.75);
    fColour = vec4(col, 1.0);
}

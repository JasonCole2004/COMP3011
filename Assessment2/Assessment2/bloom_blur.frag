#version 450 core

in vec2 texCoord;

uniform sampler2D image;
uniform bool      horizontal;

out vec4 fColour;

// 5-tap Gaussian weights
const float weight[5] = float[](0.227027, 0.194595, 0.121622, 0.054054, 0.016216);

void main()
{
    vec2 texel = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, texCoord).rgb * weight[0];

    if (horizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            result += texture(image, texCoord + vec2(texel.x * i, 0.0)).rgb * weight[i];
            result += texture(image, texCoord - vec2(texel.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            result += texture(image, texCoord + vec2(0.0, texel.y * i)).rgb * weight[i];
            result += texture(image, texCoord - vec2(0.0, texel.y * i)).rgb * weight[i];
        }
    }

    fColour = vec4(result, 1.0);
}

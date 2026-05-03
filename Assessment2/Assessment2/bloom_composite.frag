#version 450 core

in vec2 texCoord;

uniform sampler2D scene;
uniform sampler2D bloom;
uniform float     bloomStrength;

out vec4 fColour;

void main()
{
    vec3 sceneCol = texture(scene, texCoord).rgb;
    vec3 bloomCol = texture(bloom, texCoord).rgb;

    // Add the blurred bloom on top of the scene
    fColour = vec4(sceneCol + bloomCol * bloomStrength, 1.0);
}

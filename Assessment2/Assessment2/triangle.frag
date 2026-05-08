#version 450 core

in vec3 col;
in vec3 nor;
in vec3 FragPos;
in vec2 texCoord;

uniform vec3      lightPos;
uniform vec3      lightColour;
uniform vec3      camPos;
uniform float     isLight;
uniform float     isEmissive;
uniform float     hasTexture;
uniform float     useShadow;
uniform float     useNormalMap;
uniform float     alpha;
uniform sampler2D tex;
uniform sampler2D normalMap;

// Cube shadow map: sampled with a world-space direction from the sun
uniform samplerCube shadowCubeMap;
uniform float       shadowFarPlane;

// Directional light (faint starlight from a fixed direction)
uniform vec3 dirLightDir;
uniform vec3 dirLightColour;

// Spotlight (camera headlight)
uniform vec3  spotPos;
uniform vec3  spotDir;
uniform vec3  spotColour;
uniform float spotCutoff;
uniform float spotOuter;

layout(location = 0) out vec4 fColour;
layout(location = 1) out vec4 fEmissive; // only written by the sun, used for bloom

float ShadowCalc(vec3 fragPos, vec3 N)
{
    vec3  fragToLight = fragPos - lightPos;
    float currentDist = length(fragToLight);
    float closestDist = texture(shadowCubeMap, fragToLight).r * shadowFarPlane;

    // Slope-scaled bias: surfaces facing the light get a tighter bias to
    // preserve contact shadows; glancing surfaces get more to avoid acne.
    float cosTheta = max(dot(N, normalize(-fragToLight)), 0.0);
    float bias     = mix(0.1, 0.02, cosTheta);

    return currentDist - bias > closestDist ? 1.0 : 0.0;
}

void main()
{
    vec3 baseCol = (hasTexture > 0.5) ? texture(tex, texCoord).rgb : col;

    // Emissive objects (sun, skysphere) skip lighting entirely
    if (isLight > 0.5)
    {
        fColour   = vec4(baseCol, alpha);
        fEmissive = (isEmissive > 0.5) ? vec4(baseCol, alpha) : vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Resolve surface normal, using a normal map for Earth if enabled
    vec3 N = normalize(nor);
    if (useNormalMap > 0.5)
    {
        // Build TBN from the sphere's longitude direction rather than UVs
        // to avoid seam artefacts
        vec3 worldUp = vec3(0.0, 1.0, 0.0);
        vec3 T   = normalize(cross(worldUp, N));
        vec3 B   = normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);

        vec3 sampledN = texture(normalMap, texCoord).rgb * 2.0 - 1.0;
        sampledN.xy  *= 20.0; // exaggerated so terrain bumps are clearly visible
        N = normalize(TBN * sampledN);
    }

    vec3 viewDir = normalize(camPos - FragPos);
    vec3 ambient = 0.12 * lightColour;

    // Point light: the sun at the origin
    vec3  pointDir  = normalize(lightPos - FragPos);
    float pointDiff = max(dot(N, pointDir), 0.0);
    vec3  pointRefl = reflect(-pointDir, N);
    float pointSpec = pow(max(dot(viewDir, pointRefl), 0.0), 32.0);
    float shadow    = (useShadow > 0.5) ? ShadowCalc(FragPos, N) : 0.0;
    vec3  pointContrib = (1.0 - shadow) * (pointDiff * lightColour + 0.5 * pointSpec * lightColour);

    // Directional light: subtle cool tone to fill the dark sides of planets
    vec3  dirL      = normalize(-dirLightDir);
    float dirDiff   = max(dot(N, dirL), 0.0);
    vec3  dirRefl   = reflect(-dirL, N);
    float dirSpec   = pow(max(dot(viewDir, dirRefl), 0.0), 32.0);
    vec3  dirContrib = dirDiff * dirLightColour + 0.3 * dirSpec * dirLightColour;

    // Spotlight: camera headlight, diffuse only to avoid specular blow-out
    // (specular is always max when camera and light are co-located)
    vec3  spotL    = normalize(spotPos - FragPos);
    float spotDist = length(spotPos - FragPos);
    float spotAtten = 1.0 / (1.0 + 0.5 * spotDist + 0.2 * spotDist * spotDist);
    float spotDiff  = max(dot(N, spotL), 0.0);
    float theta     = dot(spotL, normalize(-spotDir));
    float epsilon   = spotCutoff - spotOuter;
    float spotIntensity = clamp((theta - spotOuter) / epsilon, 0.0, 1.0);
    vec3  spotContrib   = spotIntensity * spotAtten * spotDiff * spotColour;

    fColour   = vec4((ambient + pointContrib + dirContrib + spotContrib) * baseCol, alpha);
    fEmissive = vec4(0.0, 0.0, 0.0, 1.0);
}

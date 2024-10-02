#version 330 core
out vec4 FragColor;
 
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos; 
in vec4 FragPosLightSpace;

uniform sampler2D textureMap;
uniform sampler2D shadowMap;

uniform vec3 lightSource; 
uniform float lightIntensity;
uniform vec3 viewSource;

float ShadowCalculations(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightSource - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    // Texture values
    vec3 tex = texture(textureMap, TexCoord).rgb;

    // Ambient light
    float ambientVal= 0.2;

    // Diffused light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightSource - FragPos);
    float diffuseVal = max(dot(norm, lightDir), 0.0);

    // Specular light
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewSource - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float specularVal = specularStrength * spec;

    float shadow = ShadowCalculations(FragPosLightSpace);  

    // Applying to texture
    FragColor = vec4(lightIntensity * (ambientVal + (1.0 - shadow) * (diffuseVal + specularVal))
         * tex, 1.0);
}


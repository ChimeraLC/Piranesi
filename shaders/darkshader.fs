#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 fogColor;
uniform vec3 viewSource;
uniform vec3 viewDirection;
uniform float viewRadius;
uniform float viewRadiusOuter;
uniform sampler2D textureID;

void main()
{
    vec3 lightDir = normalize(viewSource - FragPos);

    // Check if within cone
    float theta = dot(lightDir, normalize(-viewDirection));
    float epsilon = viewRadiusOuter - viewRadius;
    float intensity = clamp((viewRadiusOuter - theta) / epsilon, 0.0, 1.0);

    // Calculate light levels
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0) * 0.8;

    float distance   = length(viewSource - FragPos);
    float atten = 1.0 / (1 + 0.03 * distance + 0.01 * (distance * distance));  

    diff *= atten * intensity;
    vec3 raw = diff * texture(textureID, TexCoord).rgb;

    // Give minimal color
    vec3 altered = vec3(max(raw.x, fogColor.x), max(raw.y, fogColor.y), max(raw.z, fogColor.z));
    FragColor = vec4(altered, 1.0);
}


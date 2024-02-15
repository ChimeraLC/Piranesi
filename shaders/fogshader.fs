#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

uniform vec3 fogColor;
uniform vec3 viewSource;
uniform sampler2D textureID;

void main()
{
    // Fog intensity
    float distance = length(viewSource - FragPos);
    float intensity = pow(distance / 50, 1);
    if (intensity > 1)
    {
        intensity = 1;
    }
    // Applying to texture
    vec3 text = texture(textureID, TexCoord).rgb;
    vec3 fog = intensity * fogColor + (1 - intensity) * text;
    FragColor = vec4(fog, 1.0);
}


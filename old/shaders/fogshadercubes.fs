#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

uniform vec3 fogColor;
uniform vec3 viewSource;
uniform sampler2D textureID;
uniform vec3 cubePos;
uniform float cubeHeight;
uniform float cubeSize;

void main()
{
    // Fog intensity
    float distance = length(viewSource - FragPos);
    float intensity = clamp(distance / 50, 0, 1);
    // Applying to texture
    vec3 text = texture(textureID, TexCoord).rgb;

    // Applying darkening to textures below cubes
    if (FragPos.x < cubePos.x + cubeSize / 2 &&
        FragPos.x > cubePos.x - cubeSize / 2 &&
        FragPos.z > cubePos.z - cubeSize / 2 &&
        FragPos.z < cubePos.z + cubeSize / 2 &&
        FragPos.y <= cubePos.y + cubeHeight) {
            text -= vec3(0.3, 0.3, 0.3);
        }
    vec3 fog = intensity * fogColor + (1 - intensity) * text;
    FragColor = vec4(fog, 1.0);
}


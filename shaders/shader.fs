#version 330 core
out vec4 FragColor;
 
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos; 

uniform sampler2D textureID;
uniform vec3 lightSource; 
uniform float lightIntensity;
uniform vec3 viewSource;

void main()
{
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

    // Applying to texture
    vec3 tex = texture(textureID, TexCoord).rgb;
    FragColor = vec4(lightIntensity * (ambientVal + diffuseVal + specularVal) * tex, 1.0);
}


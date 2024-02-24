#version 330 core
out vec4 FragColor;
 
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos; 

uniform sampler2D textureID;

void main()
{
    // Applying to texture
    FragColor = texture(textureID, TexCoord);
    
    // Color is just white
    //FragColor = vec4(0.88, 0.86, 0.82, 1.0);
}


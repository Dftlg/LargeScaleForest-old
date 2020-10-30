#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in float ourColor;

uniform sampler2D texture_diffuse1;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
   FragColor = texture(texture_diffuse1, TexCoords);
   //FragColor = vec4(ourColor,ourColor,ourColor,1.0f);
}


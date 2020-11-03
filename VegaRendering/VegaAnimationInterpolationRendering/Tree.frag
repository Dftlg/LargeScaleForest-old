#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_opacity1;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
	vec4  texColor1 = texture(texture_diffuse1, TexCoords);
	vec4  texColor2 = texture(texture_opacity1, TexCoords);
	if(texColor2.rgb == vec3(0.0,0.0,0.0))
		discard;	
    FragColor = texColor1;
}


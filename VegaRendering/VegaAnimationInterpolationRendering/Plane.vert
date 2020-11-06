#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 v2f_TexCoords;
out vec3 v2f_WorldPos;
out vec3 v2f_Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    v2f_TexCoords = aTexCoords;
	v2f_WorldPos = vec3(model * vec4(aPos,1.0));
	v2f_Normal = mat3(model) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
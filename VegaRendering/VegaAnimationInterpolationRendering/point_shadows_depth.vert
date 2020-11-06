#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in int faceId;

uniform mat4 model;
uniform int frameIndex;
uniform int treeIndex;
uniform int frameNums;
uniform int vertexNums;

layout (std430, binding=1) buffer DeformationArray
{
	vec4 u[];
};

void main()
{
	if(treeIndex < 0)
	{
		gl_Position = model * vec4(aPos, 1.0);
	}
	else
	{
		vec4 tempPos = vec4(aPos,1.0)+u[treeIndex * frameNums * vertexNums + frameIndex * vertexNums+faceId];
		 gl_Position = model * tempPos;
	}
	
}
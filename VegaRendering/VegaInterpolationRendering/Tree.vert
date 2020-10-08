#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int faceId;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8 ) in int positionIndex;

out vec2 TexCoords;

uniform int frameNums;
uniform int vertexNums;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int frameIndex;
uniform int treeIndex;

layout (std430, binding=1) buffer DeformationArray
{
	vec4 u[];
};

layout (std430, binding=2) buffer DeltaDeformationArray
{
	vec4 sum_u[];
};

void main()
{
	//vec4 tempPos=vec4(aPos,1.0)+u[treeIndex*frameNums*vertexNums+frameIndex*vertexNums+faceId];
	
	//sum_u[faceId]=u[treeIndex*frameNums*vertexNums+frameIndex*vertexNums+faceId]+sum_u[faceId];
	sum_u[positionIndex]=u[treeIndex*frameNums*vertexNums+frameIndex*vertexNums+faceId]+sum_u[positionIndex];
	vec4 tempPos=vec4(aPos,1.0)+sum_u[positionIndex];
	//vec4 tempPos=vec4(aPos,1.0)+sum_u[faceId];
//	vec4 tempPos;
//	if(gl_InstanceID<5)
//	{
//		tempPos=vec4(aPos,1.0)+u[gl_InstanceID*frameNums*vertexNums+frameIndex*vertexNums+faceId];
//	}
//	else
//	{
//		int tempInstanceIndex= gl_InstanceID / 100;
//		tempPos=vec4(aPos,1.0)+u[tempInstanceIndex*frameNums*vertexNums+frameIndex*vertexNums+faceId];
//	}
	//gl_Position = projection * view * model * instanceMatrix * tempPos;
	gl_Position = tempPos;
	TexCoords = aTexCoords;  
}

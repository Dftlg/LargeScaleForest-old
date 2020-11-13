#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in int faceId;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8 ) in int positionIndex;

uniform mat4 model;
uniform int assimpvertexNums;
uniform int planeOrTree;
uniform int frameNums;
uniform int vertexNums;

layout (std430, binding=1) buffer DeltaDeformationArray
{
	vec4 u[];
};

layout (std430, binding=2) buffer DeformationArray
{
	vec4 sum_u[];
};

layout (std430, binding=3) buffer IndexArray
{
	ivec2 treeFrameIndex[];
};

void main()
{
	if(planeOrTree < 0)
	{
		gl_Position = model * vec4(aPos, 1.0);
	}
	else
	{
		sum_u[gl_InstanceID*assimpvertexNums+positionIndex]=u[treeFrameIndex[gl_InstanceID][0]*frameNums*vertexNums+treeFrameIndex[gl_InstanceID][1]*vertexNums+faceId]+sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
		vec4 tempPos=vec4(aPos,1.0)+sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
		 gl_Position = model * instanceMatrix * tempPos;
		 //gl_Position = model * instanceMatrix tempPos;
	}
	
}
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int faceId;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8 ) in int positionIndex;

uniform mat4 model;
uniform int assimpvertexNums;
uniform int planeOrTree;
uniform int frameNums;
uniform int frameIndex;
uniform int vertexNums;
uniform float time;
uniform int sumFaceVerticesBeforeEndMesh;
uniform sampler2D waveMap;

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
		tempPos = model * instanceMatrix * tempPos;
		if(positionIndex >= sumFaceVerticesBeforeEndMesh)
		{
		//sin(texture(waveMap, aTexCoords).y
			//gl_Position = tempPos + vec4(0.0f,  pow(texture(waveMap, aTexCoords).x,6) * sin(positionIndex * frameIndex) * 0.05f,0.0f,0);
			gl_Position =  tempPos;
			//gl_Position = tempPos + vec4(0.0f,  pow(texture(waveMap, aTexCoords).x,4) * sin(frameIndex)* 0.06,0.0f,0);
		}
		else
		{
			gl_Position =  tempPos;
		}
	}
	
}
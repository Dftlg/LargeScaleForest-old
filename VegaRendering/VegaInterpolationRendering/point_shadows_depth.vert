//#version 430 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;
//layout (location = 3) in int faceId;
//layout (location = 4) in mat4 instanceMatrix;
//layout (location = 8 ) in int positionIndex;
//
//uniform mat4 model;
//uniform int assimpvertexNums;
//uniform int planeOrTree;
//uniform int frameNums;
//uniform int frameIndex;
//uniform int vertexNums;
//uniform float time;
//uniform int sumFaceVerticesBeforeEndMesh;
//uniform sampler2D waveMap;
//
//const float PI = 3.14159265359;
//
//layout (std430, binding=1) buffer DeltaDeformationArray
//{
//	vec4 u[];
//};
//
//layout (std430, binding=2) buffer DeformationArray
//{
//	vec4 sum_u[];
//};
//
//layout (std430, binding=3) buffer IndexArray
//{
//	ivec2 treeFrameIndex[];
//};
//
//void main()
//{
//	if(planeOrTree < 0)
//	{
//		gl_Position = model * vec4(aPos, 1.0);
//	}
//	else
//	{
//		sum_u[gl_InstanceID*assimpvertexNums+positionIndex]=u[treeFrameIndex[gl_InstanceID][0]*frameNums*vertexNums+treeFrameIndex[gl_InstanceID][1]*vertexNums+faceId]+sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
//		vec4 tempPos=vec4(aPos,1.0)+sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
////		tempPos = model * instanceMatrix * tempPos;
////		gl_Position =  tempPos;
//		 gl_Position = model * instanceMatrix * tempPos;
//
////		if(positionIndex >= sumFaceVerticesBeforeEndMesh)
////		{
////			float speed = 0.05;
////			float windStrong = 2 * sin(frameIndex);
////			float magnitude = (sin((tempPos.y+tempPos.x+frameIndex*PI/((28.0)*speed)))*0.15+0.15)*0.30;
////			float grassWeight = (pow(aTexCoords.x-0.5,2) + pow(1-aTexCoords.y,2))*2;
////			magnitude *= magnitude * grassWeight;
////			float d0 = sin(frameIndex * PI/(112.0 * speed)) * 3.0-1.5;
////			float d1 = sin(frameIndex * PI/(142.0 * speed)) * 3.0-1.5;
////			float d2 = sin(frameIndex * PI/(132.0 * speed)) * 3.0-1.5;
////			float d3 = sin(frameIndex * PI/(122.0 * speed)) * 3.0-1.5;
////			tempPos.x += sin((frameIndex * PI / (11.0 * speed)) + (tempPos.z + d2) + (tempPos.x + d3)) * (magnitude/2.0) * (1.0f + windStrong * 1.0f);
////			tempPos.y += sin((frameIndex * PI / (17.0 * speed)) + (tempPos.z + d2)*1.6 + (-tempPos.x + d3)*1.6) * magnitude * (1.0f + windStrong * 1.0f);
////			tempPos.z += sin((frameIndex * PI / (11.0 * speed)) + (tempPos.z + d2) + (tempPos.x + d3)) * (magnitude/2.0) * (1.0f + windStrong * 1.0f);
////			gl_Position = tempPos;
////			//gl_Position = tempPos + vec4(0.0f,  pow(texture(waveMap, aTexCoords).x,6) * sin(positionIndex * frameIndex) * 0.05f,0.0f,0);
////			//gl_Position = tempPos + vec4(0.0f,  pow((1 - (pow(aTexCoords.x-0.5,2) + pow(aTexCoords.y,2))),2) * sin(frameIndex)* 0.05,0.0f,0);
////			//gl_Position = tempPos + vec4(0.0f,  pow(texture(waveMap, aTexCoords).x,4) * sin(frameIndex)* 0.06,0.0f,0);
////		}
////		else
////		{
////			gl_Position =  tempPos;
////		}
//	}
//	
//}

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
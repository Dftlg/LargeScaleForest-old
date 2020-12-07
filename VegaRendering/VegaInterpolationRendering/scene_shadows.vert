#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int faceId;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8 ) in int positionIndex;

out vec2 v2f_TexCoords;
out vec3 v2f_WorldPos;
out vec3 v2f_Normal;

uniform int frameNums;
uniform int frameIndex;
uniform int vertexNums;
uniform int assimpvertexNums;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int planeOrTree;
uniform float time;
uniform int sumFaceVerticesBeforeEndMesh;
uniform sampler2D waveMap;

const float PI = 3.14159265359;

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
	v2f_TexCoords = aTexCoords; 
	if(planeOrTree < 0)
	{
		v2f_Normal = mat3(model) * aNormal;
		v2f_WorldPos = vec3(model * vec4(aPos,1.0));
		gl_Position = projection * view * model * vec4(aPos, 1.0);	 
	}
	else
	{
		vec4 tempPos=vec4(aPos,1.0) + sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
		v2f_Normal = mat3(model)* mat3(instanceMatrix) * aNormal;
		v2f_WorldPos = vec3(model *instanceMatrix* tempPos);
	    tempPos = projection * view * model * instanceMatrix * tempPos;
		
		if(positionIndex >= sumFaceVerticesBeforeEndMesh)
		{
		//sin(texture(waveMap, aTexCoords).y
			//gl_Position = tempPos + vec4(0.0f,  pow(texture(waveMap, aTexCoords).x,4) * sin(frameIndex)* 0.06,0.0f,0);
			//gl_Position = tempPos + vec4(0.0f,  (pow(aTexCoords.x-0.5,2) + pow(1-aTexCoords.y,2)) * sin(frameIndex)* 0.05,0.0f,0);
			//gl_Position = tempPos + vec4(0.0f,  pow((1 - (pow(aTexCoords.x-0.5,2) + pow(aTexCoords.y,2))),2) * sin(frameIndex)* 0.05,0.0f,0);
			//aColor = texture(waveMap, aTexCoords);
			gl_Position = tempPos;
		}
		else
		{
			gl_Position =  tempPos;
		}
		//gl_Position = tempPos + vec4((positionIndex/sumFaceVerticesBeforeEndMesh)*sin(time)*3.0f,(positionIndex/sumFaceVerticesBeforeEndMesh)*cos(time)*2.0f,(positionIndex/sumFaceVerticesBeforeEndMesh)*cos(time),0);
		
	}
}

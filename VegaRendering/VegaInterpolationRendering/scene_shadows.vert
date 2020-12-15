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
uniform float windStrength;
uniform int sumFaceVerticesBeforeEndMesh;

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

float smoothCurve(float x)
{
	return x * x * (3.0 - 2.0 * x);
};

float trianglewave(float x)
{
	return abs(fract(x +0.5) * 2.0 - 1.0);
};

float smoothTriangleWave(float x)
{
	return smoothCurve(trianglewave(x));
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
		vec4 tempPos=vec4(aPos,1.0);// + sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
		v2f_Normal = mat3(model)* mat3(instanceMatrix) * aNormal;
		v2f_WorldPos = vec3(model *instanceMatrix* tempPos);
	    tempPos = projection * view * model * instanceMatrix * tempPos;


		if(positionIndex >= sumFaceVerticesBeforeEndMesh)
		{
			/*************version1******************/
			vec4 windDirection = vec4(1.0,cos(time)+1,0.0,1.0);
			//float force = time * sin(2* dot(vec4(v2f_WorldPos,1.0),windDirection));
			//float leafForce = pow(v2f_WorldPos.x-windDirection.x,2)+pow(v2f_WorldPos.y-windDirection.y,2)+pow(v2f_WorldPos.z-windDirection.z,2);
			float force = sin(2* time*v2f_WorldPos.x);
            float phase=0.05;
            float flutter=2.0;
            float primaryOffset=0.08;

            float leafPhase=dot(v2f_WorldPos,vec3(1.0));
		
	        float wavesIn=time+leafPhase+phase;
            float waves=fract(wavesIn*1.975)*2.0-1.0;
            waves=0.05*smoothTriangleWave(waves);
            waves=sin(waves);
			
			vec3 bend=flutter*v2f_Normal.xyz;
            float animfade=pow((1-(pow(aTexCoords.x-0.5,2)+pow(aTexCoords.y,2))),2);
            tempPos.xyz+=((waves*bend)+(windDirection.xyz*waves))*windDirection.w*animfade;

            tempPos.xyz+=primaryOffset*windDirection.xyz*animfade*force;
            gl_Position=tempPos;
		}
		else
		{
			gl_Position =  tempPos;
		}
	}
}

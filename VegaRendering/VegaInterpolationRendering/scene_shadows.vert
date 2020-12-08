//#version 430 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;
//layout (location = 3) in int faceId;
//layout (location = 4) in mat4 instanceMatrix;
//layout (location = 8 ) in int positionIndex;
//
//out vec2 v2f_TexCoords;
//out vec3 v2f_WorldPos;
//out vec3 v2f_Normal;
//
//uniform int frameNums;
//uniform int frameIndex;
//uniform int vertexNums;
//uniform int assimpvertexNums;
//uniform mat4 projection;
//uniform mat4 view;
//uniform mat4 model;
//uniform int planeOrTree;
//uniform float time;
//uniform float windStrength;
//uniform int sumFaceVerticesBeforeEndMesh;
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
//float smoothCurve(float x)
//{
//	return x * x * (3.0 - 2.0 * x);
//}
//
//float trianglewave(float x)
//{
//	return abs(fract(x +0.5) * 2.0 - 1.0);
//}
//
//float smoothTriangleWave(float x)
//{
//	return smoothCurve(trianglewave(x));
//}
//
//void main()
//{
//	v2f_TexCoords = aTexCoords; 
//	if(planeOrTree < 0)
//	{
//		v2f_Normal = mat3(model) * aNormal;
//		v2f_WorldPos = vec3(model * vec4(aPos,1.0));
//		gl_Position = projection * view * model * vec4(aPos, 1.0);	 
//	}
//	else
//	{
//		vec4 tempPos=vec4(aPos,1.0) + sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
//		v2f_Normal = mat3(model)* mat3(instanceMatrix) * aNormal;
//		v2f_WorldPos = vec3(model *instanceMatrix* tempPos);
//	    gl_Position = projection * view * model * instanceMatrix * tempPos;
//		
////		if(positionIndex >= sumFaceVerticesBeforeEndMesh)
////		{
////			float speed = 0.8;
////			float windStrong = 1;// * sin(frameIndex*2);
////			float magnitude = (sin((frameIndex * PI / ((28.0) * speed))) * 0.05 + 0.15) * 0.27;
////			float grassWeight = (pow(aTexCoords.x-0.5,2) + pow(1-aTexCoords.y,2))*2;
////			magnitude *= magnitude * grassWeight;
////			float d0 = sin(frameIndex * PI/(112.0 * speed)) * 3.0-1.5;
////			float d1 = sin(frameIndex * PI/(142.0 * speed)) * 3.0-1.5;
////			float d2 = sin(frameIndex * PI/(132.0 * speed)) * 3.0-1.5;
////			float d3 = sin(frameIndex * PI/(122.0 * speed)) * 3.0-1.5;
////			tempPos.y += sin((frameIndex * PI / (15.0 * speed)) +d2 + d3 + (tempPos.z + tempPos.x) * (PI*2/16*3)) * magnitude;
//////			tempPos.x += sin((frameIndex * pi / (13.0 * speed)) + (position.x + d0)*0.9 + (position.z + d1)*0.9) * magnitude;
//////			tempPos.y += sin((frameIndex * pi / (16.0 * speed)) + (position.z + d2)*0.9 + (position.x + d3)*0.9) * magnitude;
//////			tempPos.z += sin((frameIndex * pi / (15.0 * speed)) + (position.z + d2) + (position.x + d3)) * (magnitude/1.0);
////
////
////
////			vec4 windDirection = vec4(0.0,1.0,0.0,1.0);
////			float phase = 1.0;//频率
////			float flutter = 0.05;//摆动幅度
////			float primaryOffset = 0.008;//所有顶点固定偏移
////			float fDetailAmp = 0.1f;
////			//顶点部分使用世界坐标位置作为随机因子
////			float leafPhase = dot(v2f_WorldPos,vec3(1.0));
////			float wavesIn = time + leafPhase;
////			float waves = fract(wavesIn * 1.975)*2.0 - 1.0;
////			waves = 0.05 * smoothTriangleWave(waves);
////			vec3 bend = flutter * fDetailAmp * aNormal.xyz;
////			//bend.y = 0.05*0.3;
////			//限制根部附近顶点的运动
////			float animfade = pow((1 - (pow(aTexCoords.x-0.5,2) + pow(aTexCoords.y,2))),2);
////			//最终混合：抖动（平滑过的三角波）+次偏移（整体来回摆动）
////			tempPos.xyz += ((waves * bend) + (windDirection.xyz * waves))*windDirection.w*animfade;
////			tempPos.xyz += primaryOffset * windDirection.xyz *animfade;
////
////
////			gl_Position = tempPos;
////		
////			//gl_Position = tempPos + vec4(0.0f,  pow(texture(waveMap, aTexCoords).x,4) * sin(frameIndex)* 0.06,0.0f,0);
////			//gl_Position = tempPos + vec4(0.0f,  (pow(aTexCoords.x-0.5,2) + pow(1-aTexCoords.y,2)) * sin(frameIndex)* 0.05,0.0f,0);
////			//gl_Position = tempPos + vec4(0.0f,  pow((1 - (pow(aTexCoords.x-0.5,2) + pow(aTexCoords.y,2))),2) * sin(frameIndex)* 0.05,0.0f,0);
////			//gl_Position =  tempPos;
////		}
////		else
////		{
////			gl_Position =  tempPos;
////		}
//	
//		//gl_Position = tempPos + vec4((positionIndex/sumFaceVerticesBeforeEndMesh)*sin(time)*3.0f,(positionIndex/sumFaceVerticesBeforeEndMesh)*cos(time)*2.0f,(positionIndex/sumFaceVerticesBeforeEndMesh)*cos(time),0);
//		
//	}
//}
//

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
uniform int vertexNums;
uniform int assimpvertexNums;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int planeOrTree;
uniform int time;

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
		vec4 tempPos=vec4(aPos,1.0)+sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
		v2f_Normal = mat3(model)* mat3(instanceMatrix) * aNormal;
		v2f_WorldPos = vec3(model *instanceMatrix* tempPos);
	    gl_Position = projection * view * model * instanceMatrix * tempPos;
	}
}
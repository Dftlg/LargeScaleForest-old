#version 430 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int sumFaceVerticesBeforeEndMesh;
uniform float time;

float M_PI=3.141592653589793238462643;

uniform float Scale[10];

in VS_OUT
{
    vec2 v2f_TexCoords;
    vec3 v2f_WorldPos;
    vec3 v2f_Normal;
}vs_in[];

in int positiontoGeo[];

out VS_OUT
{
    vec2 v2f_TexCoords;
    vec3 v2f_WorldPos;
    vec3 v2f_Normal;
}gs_out;

out vec3 col;

vec3 getNormal(vec4 positionfirst,vec4 positionsecond,vec4 positionthird)
{
    vec3 tempa=vec3(positionfirst)-vec3(positionsecond);
    vec3 tempb=vec3(positionthird)-vec3(positionsecond);
    return normalize(cross(tempa,tempb));
};

vec4 smoothCurve(vec4 x)
{
	return x * x * (3.0 - 2.0 * x);
}

float mag(vec3 vVecotr)
{
	return sqrt(vVecotr.x*vVecotr.x + vVecotr.y*vVecotr.y + vVecotr.z*vVecotr.z);
};

float twovectorangle(vec3 vfirVector,vec3 vsecVector)
{
	float tempangle = dot(vfirVector, vsecVector) / (mag(vfirVector)*mag(vsecVector));
	return acos(tempangle);
};


vec4 trianglewave(vec4 x)
{
	return abs(fract(x +0.5) * 2.0 - 1.0);
}

float sinwave(float angle,float vAmplitude, float vFrequency, float vPhase,float vYpluse)
{
    return vAmplitude * cos(angle*M_PI*vFrequency + vPhase * M_PI) + vYpluse;
};

vec4 smoothTriangleWave(vec4 x)
{
	return smoothCurve(trianglewave(x));
}

float animfadefactor(float x,float y,float centerx,float centery)
{
    float distance=sqrt(pow(x-centerx,2)+pow(y-centery,2));
    return distance;
};

void main()
{
    //计算法线
    vec3 Normal=getNormal(gl_in[0].gl_Position,gl_in[1].gl_Position,gl_in[2].gl_Position);
       vec4 tempPos;
        for (int i = 0; i <= 2; i++)
        {
             gs_out.v2f_TexCoords = vs_in[i].v2f_TexCoords;
             gs_out.v2f_WorldPos = vs_in[i].v2f_WorldPos;
             gs_out.v2f_Normal = vs_in[i].v2f_Normal;
             gl_Position = gl_in[i].gl_Position;
             if(positiontoGeo[0]>=sumFaceVerticesBeforeEndMesh)
            {
            //修改法线
                vec4 windDirection = vec4(1.0,0.0,0.0,1.0);
                float force = sin(0.1* time*vs_in[i].v2f_WorldPos.x)+1;
                float phase=0.05;
                float flutter=1.0;
                float primaryOffset=0.08;

                float leafPhase=dot(vs_in[i].v2f_WorldPos,vec3(1.0));
	            //float wavesIn=time+leafPhase+phase;
                vec2 wavesIn = vec2(time) + vec2(leafPhase,phase);
                vec4  waves = (fract(wavesIn.xxyy * vec4(1.975,0.793,0.375,0.193))*2.0 - 1.0);
                waves = 0.08 * smoothTriangleWave(waves);
                vec2 waveSum = waves.xz +waves.yw;

                //vec3 bend = vec3(flutter) *  Normal.xyz;
                 vec3 bend = vec3(flutter) *  vs_in[i].v2f_Normal.xyz;
                //限制根部附近顶点的运动
                float animfade = pow((1 - (pow(vs_in[i].v2f_TexCoords.x-0.5,2) + pow(vs_in[i].v2f_TexCoords.y,2))),2);

                float angle=twovectorangle(Normal,windDirection.xyz);
                float area=1*cos(angle);

                tempPos=gl_in[i].gl_Position;
                //最终混合：抖动（平滑过的三角波）+次偏移（整体来回摆动）
			/////(vec3(波*弯曲）+ vec3(风向*波))*float*float
                tempPos.xyz +=  ((waveSum.xyx * bend) + 0.3 * (windDirection.xyz * waveSum.y*area))*windDirection.w*animfade;

                tempPos.xyz += 0.1 * primaryOffset * windDirection.xyz * animfade * force;
                 gl_Position=tempPos;
               gs_out.v2f_Normal=Normal;     
            }
              EmitVertex();
         }
    EndPrimitive(); 
   
   
     
}

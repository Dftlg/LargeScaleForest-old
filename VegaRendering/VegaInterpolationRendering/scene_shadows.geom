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

float smoothCurve(float x)
{
	return x * x * (3.0 - 2.0 * x);
};

float mag(vec3 vVecotr)
{
	return sqrt(vVecotr.x*vVecotr.x + vVecotr.y*vVecotr.y + vVecotr.z*vVecotr.z);
};

float twovectorangle(vec3 vfirVector,vec3 vsecVector)
{
	float tempangle = dot(vfirVector, vsecVector) / (mag(vfirVector)*mag(vsecVector));
	return acos(tempangle);
};


float trianglewave(float x,float scale)
{
	return abs(fract(scale*(x +0.5)) * 2.0 - 1.0);
};

float sinwave(float angle,float vAmplitude, int vFrequency, float vPhase,int vYpluse)
{
    return vAmplitude * cos(angle*M_PI*vFrequency + vPhase * M_PI) + vYpluse;
};

float smoothTriangleWave(float x,float scale)
{
    float temp= sinwave(fract(time),0.5,1,0,0);
    return smoothCurve(temp);
	//return smoothCurve(trianglewave(x,scale)+temp);
};



float animfadefactor(float x,float y,float centerx,float centery)
{
    float distance=sqrt(pow(x-centerx,2)+pow(y-centery,2));
    return distance;
};

void main()
{
    //计算法线
    vec3 Normal=getNormal(gl_in[0].gl_Position,gl_in[1].gl_Position,gl_in[2].gl_Position);
       
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
                float phase=0.05;
                float flutter=2.0;
                float primaryOffset=0.08;

                float leafPhase=dot(vs_in[i].v2f_WorldPos,vec3(1.0));
	            float wavesIn=time+leafPhase+phase;
                
                float waves=fract(wavesIn*1.975)*2.0-1.0;

//                for(int j=1;j<=10;j++)
//                {
//                    if(positiontoGeo[0]>sumFaceVerticesBeforeEndMesh+3591*(j-1)&&positiontoGeo[0]<sumFaceVerticesBeforeEndMesh+3591*j)
//                        waves=0.05*smoothTriangleWave(waves,Scale[j]);
//                }
                 waves=0.1*smoothTriangleWave(waves,0.5);

                vec3 bend=flutter*Normal.xyz;
            //根部不动
                
                float angle=twovectorangle(Normal,windDirection.xyz);
                float area=1*cos(angle);


                col=vec3(waves,0,0);


                float animfade=animfadefactor(vs_in[i].v2f_TexCoords.x,vs_in[i].v2f_TexCoords.y,0.5,1);

                gs_out.v2f_Normal=Normal;
                //gl_Position = gl_in[i].gl_Position+vec4(1*((waves*bend)*windDirection.w*animfade),0);
                //gl_Position = gl_in[i].gl_Position+vec4(1*((windDirection.xyz*waves)*windDirection.w*animfade),0);
                gl_Position = gl_in[i].gl_Position+vec4(0.5*(0.2*(waves*bend)+(windDirection.xyz*waves))*windDirection.w*animfade,0);
            }
              EmitVertex();
         }
    EndPrimitive(); 
   
   
     
}

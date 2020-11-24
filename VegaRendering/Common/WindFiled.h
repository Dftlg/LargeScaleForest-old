#pragma once
//#include <glm/vec3.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <io.h>
#include <assimp/mesh.h>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "ExtraTool.h"
#include "common.h"
#include <fstream>
#include<sstream>

struct SWaveFunctionPara
{
	//振幅
	int Amplitude;
	
	//频率
	int Frequency;

	//相位
	double Phase;
	
	//偏距
	int Ypluse;

    //是否进行脉冲函数处理
    int Pulse;

    //对与脉冲函数中下方的函数直线进行上下的浮动调整
    int DropNumber;

	SWaveFunctionPara() = default;
	SWaveFunctionPara(int vAmplitude, int vFrequency, double vPhase, int vYpluse,int vPulse=0,int vDropNumber=0)
	{
		Amplitude = vAmplitude;
		Frequency = vFrequency;
		Phase = vPhase;
		Ypluse = vYpluse;
        Pulse = vPulse;
        DropNumber = vDropNumber;
	}
};

class CWindField
{
public:
	CWindField()=default;
	//direction wind
    //最后一个参数为场景中的风场方向
	CWindField(const int vSize,const std::vector<SWaveFunctionPara> vSwavePara,const int vWavelength,Common::SForceDirection vWindDirection,Common::SForceDirection vTreeRotationDirection);
	//specific wind source
	CWindField(const glm::vec3 vWindCenter, const std::vector<SWaveFunctionPara> vSwavePara, int AmplitudeInWindCenter, int Sphere4Influence);
	~CWindField()=default;

    Common::SForceDirection caculateRelativeDirection(Common::SForceDirection &vWindDirection, Common::SForceDirection &vTreeRotationDirection);
	std::vector<int> getDirectionWindForces() { return m_Forces[0]; };
	std::vector<Common::SForceDirection> getDirectionWindDirection() { return m_RelativeDirectionOfWindAndTree[0]; };

	std::vector<int> getSpecificWindSourceForces(glm::vec3 vObjectPosition);

    void saveForces2File(const std::string filePath);

private:
	glm::vec3 m_WindCenter;
	int WindInfluenceField;
	std::vector<SWaveFunctionPara> m_WavePara;
	int AmplitudeWindCenter;

	std::vector<std::vector<int>> m_Forces;
	std::vector<std::vector<Common::SForceDirection>> m_RelativeDirectionOfWindAndTree;
};

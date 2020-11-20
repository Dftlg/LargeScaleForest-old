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

struct SWaveFunctionPara
{
	//Õñ·ù
	int Amplitude;
	
	//ÆµÂÊ
	int Frequency;

	//ÏàÎ»
	int Phase;
	
	//Æ«¾à
	int Ypluse;
};

class CWindField
{
public:
	CWindField()=default;
	//direction wind
	CWindField(const int vSize,const std::vector<SWaveFunctionPara> vSwavePara,const int vWavelength);
	//specific wind source
	CWindField(const glm::vec3 vWindCenter, const std::vector<SWaveFunctionPara> vSwavePara, int AmplitudeInWindCenter, int Sphere4Influence);
	~CWindField()=default;

	std::vector<int> getDirectionWindForces() { return Forces[0]; };

	std::vector<int> getSpecificWindSourceForces(glm::vec3 vObjectPosition);

private:
	glm::vec3 m_WindCenter;
	int WindInfluenceField;
	std::vector<SWaveFunctionPara> m_WavePara;
	int AmplitudeWindCenter;

	std::vector<std::vector<int>> Forces;
};

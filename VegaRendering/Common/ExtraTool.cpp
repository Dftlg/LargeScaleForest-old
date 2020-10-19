
#include "ExtraTool.h"

int RandomGenerate()
{
	LARGE_INTEGER seed;
	QueryPerformanceFrequency(&seed);
	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);
	int randomNumber = rand();
	//return (randomNumber % 360);
	if (randomNumber % 2 == 0) return (randomNumber % 90) * 1;
	return (randomNumber % 90)*-1;
}

std::vector<int> GenerateSamplingForce(int vSize, int vAmplitude, int vFrequency, double vPhase,int vYpluse)
{
	double angle = 0.0;
	std::vector<int> tempForces;
	for (int i = 0; i < vSize; i++)
	{
		tempForces.push_back(int(vAmplitude * cos(angle*vFrequency + vPhase) + vYpluse));
		angle += (2*M_PI) / vSize;
	}
	return tempForces;
}

std::vector<int> GetForceConfigurate(const std::string& vFilePath)
{
	std::vector<std::string> ForceConfig;
	std::vector<int> tempConfig;
	int forcePos = vFilePath.find("force");
	std::string tempFile = vFilePath.substr(forcePos+5);
	boost::split(ForceConfig, tempFile, boost::is_any_of(","), boost::token_compress_off);
	std::vector<std::string>::iterator it;
	for (it = ForceConfig.begin(); it != ForceConfig.end(); ++it)
	{
		tempConfig.push_back(std::stoi(*it));
	}
	return tempConfig;
	
}
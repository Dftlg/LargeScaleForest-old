
#include "ExtraTool.h"

int RandomGenerate()
{
	LARGE_INTEGER seed;
	QueryPerformanceFrequency(&seed);
	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);
	int randomNumber = rand();
	//return (randomNumber % 360);
	if (randomNumber % 2 == 0) return (randomNumber % 60) * 1;
	return (randomNumber % 60)*-1;
}

glm::vec3 GenerateRamdomScale()
{
	LARGE_INTEGER seed;
	QueryPerformanceFrequency(&seed);
	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);
	int randomNumber = rand();
	float scalenumber = randomNumber % 1000 / (float)1000;
	while (scalenumber < 0.6)
	{
		QueryPerformanceFrequency(&seed);
		QueryPerformanceCounter(&seed);
		srand(seed.QuadPart);
		int randomNumber = rand();
		scalenumber = randomNumber % 1000 / (float)1000;
	}
	return glm::vec3(scalenumber, scalenumber, scalenumber);
}

std::vector<std::pair<double,double>> RandomTreePositionGenerate(int vTreeNumber)
{
	std::vector<std::pair<int, int>> tempTreePosition;
	std::vector<std::pair<double, double>> tempTreedoublePosition(vTreeNumber);
	while (tempTreePosition.size()!=vTreeNumber)
	{
		LARGE_INTEGER seed;
		QueryPerformanceFrequency(&seed);
		QueryPerformanceCounter(&seed);
		srand(seed.QuadPart);
		std::pair<int, int> temppair(rand() % vTreeNumber, rand() % vTreeNumber);
		tempTreePosition.push_back(temppair);

		std::sort(tempTreePosition.begin(), tempTreePosition.end());
		tempTreePosition.erase(std::unique(tempTreePosition.begin(), tempTreePosition.end()), tempTreePosition.end());
	}
	for (auto i = 0; i < tempTreePosition.size(); i++)
	{
		std::default_random_engine e;
		LARGE_INTEGER seed;
		QueryPerformanceFrequency(&seed);
		QueryPerformanceCounter(&seed);
		e.seed(seed.QuadPart);
		std::uniform_real_distribution<double> u(0, 1);
		tempTreedoublePosition[i].first = tempTreePosition[i].first + u(e);
		tempTreedoublePosition[i].second = tempTreePosition[i].second + u(e);
	}
	return tempTreedoublePosition;
}

//采样频率，振幅，频率，相位，偏距
std::vector<int> GenerateSamplingForce(int vSize, int vAmplitude, int vFrequency, double vPhase,int vYpluse,int wavelength)
{
	double angle = 0.0;
	std::vector<int> tempForces;
	for (int i = 0; i < vSize; i++)
	{
		tempForces.push_back(int(vAmplitude * cos(angle*vFrequency + vPhase * M_PI) + vYpluse));
		angle += (wavelength*M_PI) / vSize;
	}
	return tempForces;
}

std::vector<double> GetForceConfigurate(const std::string& vFilePath)
{
	std::vector<std::string> ForceConfig;
	std::vector<double> tempConfig;
	int forcePos = vFilePath.find("force");
	std::string tempFile = vFilePath.substr(forcePos+5);
	boost::split(ForceConfig, tempFile, boost::is_any_of(","), boost::token_compress_off);
	std::vector<std::string>::iterator it;
	for (it = ForceConfig.begin(); it != ForceConfig.end(); ++it)
	{
		tempConfig.push_back(std::stof(*it));
	}
	return tempConfig;
	
}

//输入数据，控制范围在数据的周围每次扩大10例如：10,100,0.1,0.001，控制其范围的倍率
double OneNumberRangeError(float vNumber,int vControlFloatPosition,int vRange)
{
	float tempNumber = abs(vNumber);
	int BaseNumber = 0;
	if (tempNumber < 1)
	{
		while (tempNumber < 1)
		{
			tempNumber *= 10;
			BaseNumber++;
		}
		return (1.0 / ((BaseNumber + vControlFloatPosition) * 10))*vRange;
	}
	else if (tempNumber >= 1)
	{
		while (tempNumber >= 1)
		{
			tempNumber /= 10;
			BaseNumber++;
		}
		return ((BaseNumber - vControlFloatPosition) * 10)*vRange;
	} 
}
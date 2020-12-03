
#include "ExtraTool.h"

int MaxElement(std::vector<int> &vDataSet)
{
	int max=*(std::max_element(vDataSet.begin(), vDataSet.end()));
	return max;
}

int MinElement(std::vector<int> &vDataSet)
{
	int min=*(std::min_element(vDataSet.begin(), vDataSet.end()));
	return min;
}

bool isDataExist(int number, std::vector<int> &vDataSet)
{
	for (auto tempdata : vDataSet)
		if (tempdata == number)
			return true;
	return false;
}

//改成0-360
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
	while (scalenumber < 0.92)
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
	std::vector<int> tempTreeXPosition;
	std::vector<int> tempTreeYPosition;
	std::vector<std::pair<int, int>> tempTreePosition;
	std::vector<std::pair<double, double>> tempTreedoublePosition(vTreeNumber);


	for (int i = 0; i < vTreeNumber; i++)
	{
		tempTreeXPosition.push_back(i);
		tempTreeYPosition.push_back(i);
	}

	std::random_shuffle(tempTreeXPosition.begin(), tempTreeXPosition.end());
	std::random_shuffle(tempTreeYPosition.begin(), tempTreeYPosition.end());

	for (int i = 0; i < vTreeNumber; i++)
	{
		tempTreedoublePosition[i].first = tempTreeXPosition[i];
		tempTreedoublePosition[i].second = tempTreeYPosition[i];
	}

	//while (tempTreePosition.size()!=vTreeNumber)
	//{
	//	LARGE_INTEGER seed;
	//	QueryPerformanceFrequency(&seed);
	//	QueryPerformanceCounter(&seed);
	//	srand(seed.QuadPart);
	//	int tempfirst = rand() % vTreeNumber;
	//	//std::pair<int, int> temppair(rand() % vTreeNumber, rand() % vTreeNumber);
	//	QueryPerformanceFrequency(&seed);
	//	QueryPerformanceCounter(&seed);
	//	srand(seed.QuadPart);
	//	int tempsecond = rand() % vTreeNumber;
	//	std::pair<int, int> temppair(tempfirst, tempsecond);



	//	tempTreePosition.push_back(temppair);

	//	//std::sort(tempTreePosition.begin(), tempTreePosition.end());
	//	tempTreePosition.erase(std::unique(tempTreePosition.begin(), tempTreePosition.end()), tempTreePosition.end());
	//}
	/*for (auto i = 0; i < tempTreePosition.size(); i++)
	{
		std::default_random_engine e;
		LARGE_INTEGER seed;
		QueryPerformanceFrequency(&seed);
		QueryPerformanceCounter(&seed);
		e.seed(seed.QuadPart);
		std::uniform_real_distribution<double> u(0, 1);
		tempTreedoublePosition[i].first = tempTreePosition[i].first + u(e);
		tempTreedoublePosition[i].second = tempTreePosition[i].second + u(e);
	}*/
	/*for (auto i = 0; i < tempTreePosition.size(); i++)
	{
		tempTreedoublePosition[i].first = tempTreePosition[i].first;
		tempTreedoublePosition[i].second = tempTreePosition[i].second;
	}*/
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

std::vector<std::vector<double>> GetForceConfigurate(const std::string& vFilePath,const std::string &vExternFile,int &vTheta,int &vPhi)
{
	std::vector<std::string> ForceConfig;
	std::vector <std::vector<double>> tempConfig;
	int thetaPos = vFilePath.find("the");
	int phiPos = vFilePath.find("phi");
	int forcePos = vFilePath.find("force");
	int IndexPos = vFilePath.find("Index");
	int endPos = vFilePath.find(".");
	vTheta = std::stoi(vFilePath.substr(thetaPos + 3, phiPos - thetaPos - 3));
	vPhi = std::stoi(vFilePath.substr(phiPos + 3, forcePos - phiPos - 3));
	int vforceNumber = std::stoi(vFilePath.substr(forcePos + 5, IndexPos - forcePos - 5));

	std::string forceFileName = vExternFile +"/"+ vFilePath.substr(IndexPos + 5, endPos - IndexPos - 5) + ".txt";

	std::ifstream positionFile(forceFileName, std::ios::in);
	std::string lineString;
	while (getline(positionFile, lineString))
	{
		boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
		std::vector<double> tempCon;
		std::vector<std::string>::iterator it;
		for (it = ForceConfig.begin(); it != ForceConfig.end(); ++it)
		{
			tempCon.push_back(std::stof(*it));
		}
		tempConfig.push_back(tempCon);
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
		return (1.0 / pow(10,(BaseNumber + vControlFloatPosition)))*vRange;
	}
	else if (tempNumber >= 1)
	{
		while (tempNumber >= 1)
		{
			tempNumber /= 10;
			BaseNumber++;
		}
		return (pow(10,(BaseNumber-1 - vControlFloatPosition)))*vRange;
	} 
}


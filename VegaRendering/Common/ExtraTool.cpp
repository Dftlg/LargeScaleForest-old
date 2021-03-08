
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

		//std::sort(tempTreePosition.begin(), tempTreePosition.end());
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

//opengl z轴向外与球坐标z相反
std::vector<std::pair<double, double>> StableTreePositionGenerate(int vTreesNumber)
{
    std::vector<std::pair<double, double>> tempTreedoublePosition;
   /* for (int i = 0; i < vTreesNumber; i++)
    {
        tempTreedoublePosition.push_back(std::make_pair(0, i+1));
    }*/
    tempTreedoublePosition.push_back(std::make_pair(2, 0));
   /* tempTreedoublePosition.push_back(std::make_pair(-2, 0));
    tempTreedoublePosition.push_back(std::make_pair(0, 2));
    tempTreedoublePosition.push_back(std::make_pair(0, -2));

    tempTreedoublePosition.push_back(std::make_pair(5, 0));
    tempTreedoublePosition.push_back(std::make_pair(-5, 0));
    tempTreedoublePosition.push_back(std::make_pair(0, 5));
    tempTreedoublePosition.push_back(std::make_pair(0, -5));

    tempTreedoublePosition.push_back(std::make_pair(7, 0));
    tempTreedoublePosition.push_back(std::make_pair(-7, 0));
    tempTreedoublePosition.push_back(std::make_pair(0, 7));
    tempTreedoublePosition.push_back(std::make_pair(0, -7));*/
  
    tempTreedoublePosition.push_back(std::make_pair(10, 0));
  /*  tempTreedoublePosition.push_back(std::make_pair(-10, 0));
    tempTreedoublePosition.push_back(std::make_pair(0, 10));
    tempTreedoublePosition.push_back(std::make_pair(0, -10));*/

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

std::vector<int> LineSamplineForce(std::pair<int, int>& vfirstPoint, std::pair<int, int>& vSecondPoint)
{
    std::vector<int> tempYForce;
    int k = (vSecondPoint.second - vfirstPoint.second) / (vSecondPoint.first - vfirstPoint.first);
    for (int xNumber = 0; xNumber < vSecondPoint.first - vfirstPoint.first; xNumber++)
    {
        tempYForce.push_back(k*xNumber + vfirstPoint.second);
    }
    return tempYForce;
}

std::vector<int> GenerateLineForce(int vFrameNumber,std::vector<std::pair<int,int>>& vForceConfig)
{
    if (vForceConfig.back().first != vFrameNumber)
    {
        vForceConfig.push_back(std::make_pair(vFrameNumber, vForceConfig[vForceConfig.size()-1].second));
    }
    std::vector<int> tempForce;
    for (int i = 0; i < vForceConfig.size()-1; i++)
    {
        std::vector<int> temp=LineSamplineForce(vForceConfig[i], vForceConfig[i+1]);
        tempForce.insert(tempForce.end(), temp.begin(), temp.end());
    }
    return tempForce;
}

std::vector<std::vector<double>> GetForceConfigurate(const std::string& vFilePath,const std::string &vExternFile,int &vTheta,int &vPhi,float &vScale, std::vector<std::pair<int, int>>& voLineForceConfig)
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

    bool isLineForce=false;

	std::ifstream positionFile(forceFileName, std::ios::in);
	std::string lineString;
	while (getline(positionFile, lineString))
	{
        if (lineString == "initForce:")
        {
            isLineForce = true;
            break;
        }
        if (lineString == "scale:")
        {
            getline(positionFile, lineString);
            vScale = std::stof(lineString);
        }
		boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
		std::vector<double> tempCon;
		std::vector<std::string>::iterator it;
		for (it = ForceConfig.begin(); it != ForceConfig.end(); ++it)
		{
			tempCon.push_back(std::stof(*it));
		}
		tempConfig.push_back(tempCon);
	}
    if (isLineForce == true)
    {
        getline(positionFile, lineString);
        voLineForceConfig.push_back(std::make_pair(0, std::stoi(lineString)));
        getline(positionFile, lineString);
        while (getline(positionFile, lineString))
        {
            boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
            std::pair<int, int> tempPair(std::stoi(ForceConfig[0]), std::stoi(ForceConfig[1]));
            voLineForceConfig.push_back(tempPair);
        }
    }
	return tempConfig;
	
}

//std::vector<std::pair<int,int>> GetLineForceConfigurate(const std::string &vExternFile)
//{
//    std::vector<std::string> ForceConfig;
//    std::vector <std::pair<int,int>> tempConfig;
//
//    std::ifstream positionFile(vExternFile, std::ios::in);
//    std::string lineString;
//    getline(positionFile, lineString);
//    if (lineString == "initForce:")
//    {
//        getline(positionFile, lineString);
//        tempConfig.push_back(std::make_pair(0, std::stoi(lineString)));
//    }
//    getline(positionFile, lineString);
//    while (getline(positionFile, lineString))
//    {
//        boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
//        std::pair<int, int> tempPair(std::stoi(ForceConfig[0]), std::stoi(ForceConfig[1]));
//        tempConfig.push_back(tempPair);
//    }
//    return tempConfig;
//
//}

void GetStemAndLeafStreePointsConfigurate(const std::string& vFilePath, std::pair<std::vector<int>, std::vector<float>>&vStreePointAndScale,std::vector<std::pair<int,int>>& vForceDeviationDirection,std::vector<float>&vFrequency,std::vector<float>&vPhase)
{
    std::pair<std::vector<int>, std::vector<float>> temp;
    std::vector<int> tempStreePoints;
    std::vector<float> tempStreePointsScales;
    std::vector<std::string> StreePointsConfig;
    std::ifstream positionFile(vFilePath, std::ios::in);
    std::string lineString;
    getline(positionFile, lineString);
    if (lineString == "stem:")
    {
        getline(positionFile, lineString);
        boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
        std::vector<std::string>::iterator it;
        for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
        {
            tempStreePoints.push_back(std::stoi(*it));
        }
    }
    StreePointsConfig.clear();
    getline(positionFile, lineString);
    if (lineString == "leaf:")
    {
        getline(positionFile, lineString);
        if (lineString != "")
        {
            boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
            std::vector<std::string>::iterator it;
            for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
            {
                tempStreePoints.push_back(std::stoi(*it));
            }
        }    
    }
    StreePointsConfig.clear();
    getline(positionFile, lineString);
    if (lineString == "leafScale:")
    {
        getline(positionFile, lineString);
        boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
        std::vector<std::string>::iterator it;
        for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
        {
            tempStreePointsScales.push_back(std::stof(*it));
        }
    }
    StreePointsConfig.clear();
    getline(positionFile, lineString);
    if (lineString == "leafFrequency:")
    {
        getline(positionFile, lineString);
        boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
        std::vector<std::string>::iterator it;
        for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
        {
            vFrequency.push_back(std::stof(*it));
        }
    }
    StreePointsConfig.clear();
    getline(positionFile, lineString);
    if (lineString == "leafPhase:")
    {
        getline(positionFile, lineString);
        boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
        std::vector<std::string>::iterator it;
        for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
        {
            vPhase.push_back(std::stof(*it));
        }
    }
    StreePointsConfig.clear();
    getline(positionFile, lineString);
    if (lineString == "leafDeviationDirection:")
    {
        getline(positionFile, lineString);
        boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
        for (int i = 0; i < StreePointsConfig.size() / 2; i++)
        {
            std::pair<int, int> tempPair;
            tempPair.first =std::stoi(StreePointsConfig[2*i]);
            tempPair.second = std::stoi(StreePointsConfig[2 * i + 1]);
            vForceDeviationDirection.push_back(tempPair);
        }
    }
    if (tempStreePoints.size() != tempStreePointsScales.size())
        std::cout << "pullVertex GetStemAndLeafStreePointsConfigurate doesn't match" << std::endl;
    else
    {
        vStreePointAndScale.first = tempStreePoints;
        vStreePointAndScale.second = tempStreePointsScales;
    }


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
    return 0;
}

//计算范围从+0到360
//坐标系图片参考文档
//opengl点的x,yz坐标系与theta和phi坐标系z轴相同
void TransformCartesianCorrdinate2SphericalCorrdinate(glm::vec3 &vStartPoint, glm::vec3 &vEndPoint, double &voRadius, int& voTheta, int& voPhi)
{
    glm::vec3 tempVector =glm::normalize(vEndPoint - vStartPoint);
    //////////////////
    //tempVector.z = -tempVector.z;
    if (tempVector.x == 0)
        tempVector.x = FLT_MIN;
    //std::cout << tempVector.x << " " << tempVector.y << " " << tempVector.z << std::endl;
    voRadius = sqrt(tempVector.x*tempVector.x + tempVector.y*tempVector.y + tempVector.z*tempVector.z);

    double ThetaRadian = asin(tempVector.y / voRadius);
    double PhiRadian = atan2(tempVector.z , tempVector.x);

    voTheta = 180 * ThetaRadian / M_PI;
    if (voTheta < 0)
    {
        voTheta += 360;
    }
    voPhi = 180 * PhiRadian / M_PI;
    if (voPhi < 0)
    {
        voPhi += 360;
    }
}


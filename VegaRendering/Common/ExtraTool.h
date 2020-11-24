#pragma once    
#include<iostream>
#include <fstream>
#include<sstream>
#include<ctime>
#include<random>
#include<math.h>
#include <Windows.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include<sstream>
#include<algorithm>
#include <glm/glm.hpp>

#define M_PI 3.1415926

int MaxElement(std::vector<int> &vDataSet);

int MinElement(std::vector<int> &vDataSet);

bool isDataExist(int number, std::vector<int> &vDataSet);

int RandomGenerate();

std::vector<int> GenerateSamplingForce(int vSize, int vAmplitude, int vFrequency, double vPhase, int vYpluse, int wavelength = 2);

std::vector<std::vector<double>> GetForceConfigurate(const std::string & vFilePath, const std::string &vExternFile, int &vTheta, int &vPhi);

std::vector<std::pair<double, double>> RandomTreePositionGenerate(int vTreeNumber);

glm::vec3 GenerateRamdomScale();

double OneNumberRangeError(float vNumber, int vControlFloatPosition=0, int vRange=1);
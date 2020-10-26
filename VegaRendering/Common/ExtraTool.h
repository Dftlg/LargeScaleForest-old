#pragma once    
#include<iostream>
#include<ctime>
#include<math.h>
#include <Windows.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include<sstream>

#define M_PI 3.1415926

int RandomGenerate();

std::vector<int> GenerateSamplingForce(int vSize, int vAmplitude, int vFrequency, double vPhase, int vYpluse, int wavelength = 2);

std::vector<double> GetForceConfigurate(const std::string & vFilePath);

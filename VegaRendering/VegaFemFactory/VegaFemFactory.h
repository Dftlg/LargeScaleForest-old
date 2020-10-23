#pragma once

#include <iostream>
#include <fstream>
#include<sstream>
#include <assert.h>
#include <thread>
#include <time.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "common.h"
#include "ExtraTool.h"
#include "ModelDeformationTransform.h"

class CVegaFemFactory
{
public:

	CVegaFemFactory() = default;
	CVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile, const std::string &vCorrectDeformationUVertexIndex);
	~CVegaFemFactory() = default;

	void readCorrectUdeformationIndex(const std::string & vFilePath);
	void readKVFFileData(const std::string & vFile, Common::SFileFrames & vFileFrame);
	//void readUdeformationData(const std::string & vFile, Common::SFileFrames &vFileFrame);
	void readFilePath4Directory(const std::string & vDirectoryName);
	void write2File(const std::string& vPath, std::vector <std::pair<int, double>>& vOutputData);
	void readFramesDeformationData(std::vector<Common::SFileFrames>& vSearchFrames, int vSearchConnectionIndex);
	void readFramesDeformationDataBasedFilesIndex(std::vector<std::pair<int, int>>&vFilesAndFramesIndexSequence, std::vector<std::vector<glm::vec3>> &voMatchedFramesData);
	void readDeformationDataByMutileThread(Common::SFileFrames& vBaseFileFramesStruct, const std::string& vFilePath, int vSFileFramesIndex);
	void readDeformationDataInOneThread(Common::SFileData& vFileData, int vTid, const std::string& vFilePath, long long vBlockSize);
	void addSeekgOfEachFramesBlock(const std::string& vFilePath);
	std::vector<Common::SFileDataGroup> getConnectedFemMutileDeformation(int vConnectionIndex, int vTimestep);
	//根据文件的绝对路径取文件名字,如：positionthe0phi0force500,500,500
	std::string getFileName(const std::string & vFileDirectory);
	//返回一个形变文件
	Common::SFileFrames getFileFrames(int vIndex) { return m_FilesData[vIndex]; }
	void setDeformationStateFromFileName();
	CModelDeformationTransform*  getModelTransformStruct() { return m_ModelTransformStruct;}
	std::vector<Common::SFileFrames> searchFileFramesOnAnimation(const int vTheta, const int vPhi, const std::vector<int> & vForceFluctuationSequence);
	std::vector<Common::SFileFrames> searchFileFrameOnAttribute();
	std::vector<std::vector<glm::vec3>> objDeformation(std::pair<int, int> vForceDirection, std::vector<int> vForceFluctuationSequence);
	void cleanSFileDataGroup(int vConnectionIndex, int vTimestep);
	//查找检索标准
	void searchMatchedFrameSegment(std::vector<std::vector<glm::vec3>>& voMatchedFramesSequences, Common::SpKVFData& voSpKVData, std::vector<int> &vExtraForces, bool vIsFirstFrame);
	
	void searchMatchedDeformationFrames(std::vector<glm::vec3> & vFrameUDeformationData);

	inline float addDistance(float vorigin) { return vorigin + 0.0005; };
	inline float minusDistance(float vorigin) { return vorigin - 0.0005; };
	inline int distanceError(glm::vec3 vorigin, glm::vec3 vinput) 
	{ 
		if ((addDistance(vorigin.x) > vinput.x&& minusDistance(vorigin.x) < vinput.x) &&
			(addDistance(vorigin.y) > vinput.y&& minusDistance(vorigin.y) < vinput.y) &&
			addDistance(vorigin.z) > vinput.z&& minusDistance(vorigin.z) < vinput.z)
			return 1;
		return 0;
	};
	inline int SquareError(int vFirstNumber, int vSecondNumber)
	{
		return (vFirstNumber - vSecondNumber)*(vFirstNumber - vSecondNumber);
	};
	inline double SquareError(double vFirstNumber, double vSecondNumber)
	{
		return (vFirstNumber - vSecondNumber)*(vFirstNumber - vSecondNumber);
	};
	inline bool AbsError(double vFirstNumber, double vSecondNumber,double vJudgeRange)
	{
		return (abs(vFirstNumber - vSecondNumber) < vJudgeRange);
	};
	inline bool AbsError(glm::vec3 vFirstNumber, glm::vec3 vSecondNumber, double vJudegRange)
	{
		if (abs(vFirstNumber.x - vSecondNumber.x) < vJudegRange && 
			abs(vFirstNumber.y - vSecondNumber.y) < vJudegRange &&
			abs(vFirstNumber.z - vSecondNumber.z) < vJudegRange)
			return true;
		return false;
	};
	inline double GaussianFunction(double vVariable,double vSigma=1, double vMiu=0)
	{
		return (1 / (vSigma*sqrt(2 * Common::Pi))*exp(-0.5*pow((vVariable - vMiu) / vSigma, 2)));
	}


private:

	void __getFileSeekDirOfEachBlock(const std::string& vFilePath, std::vector<long long>& vBlock);
	//路径下面的每个文件的绝对路径集合，如：D:\GraduationProject\Vega\models\8.10\test\positionthe0phi0force500,500,500.txt
	std::vector<std::string> m_FilePathList;
	//形变文件的集合
	std::vector<Common::SFileFrames> m_FilesData;
	std::vector<Common::SConnectedFemFiles> m_AllReallyLoadConnectedFem;
	//可以看作是一个obj的model对象，有mesh集合，以及group组集合
	CModelDeformationTransform* m_ModelTransformStruct;

	//需要匹配的某些体素下的Object顶点
	std::vector<std::vector<int>> m_CorrectDeformationIndex;
};
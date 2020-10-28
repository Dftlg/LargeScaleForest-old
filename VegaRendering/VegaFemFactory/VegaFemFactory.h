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
	void readUdeformationData(const std::string & vFile, Common::SFileFrames &vFileFrame);
	void readFilePath4Directory(const std::string & vDirectoryName);
	void write2File(const std::string& vPath, std::vector <std::pair<int, double>>& vOutputData, std::vector <std::pair<int, double>>& vForceFrame, std::vector <std::pair<int, double>>& vKVfFrame, int vFrameIndex);
	void readFramesDeformationData(std::vector<Common::SFileFrames>& vSearchFrames, int vSearchConnectionIndex);
	void readFramesDeformationDataBasedFilesIndex(std::vector<std::pair<int, int>>&vFilesAndFramesIndexSequence, std::vector<std::vector<glm::vec3>> &voMatchedFramesData);
	void readDeformationDataByMutileThread(Common::SFileFrames& vBaseFileFramesStruct, const std::string& vFilePath, int vSFileFramesIndex);
	void readDeformationDataInOneThread(Common::SFileData& vFileData, int vTid, const std::string& vFilePath, long long vBlockSize);
	void addSeekgOfEachFramesBlock(const std::string& vFilePath);
	std::vector<Common::SFileDataGroup> getConnectedFemMutileDeformation(int vConnectionIndex, int vTimestep);
	//�����ļ��ľ���·��ȡ�ļ�����,�磺positionthe0phi0force500,500,500
	std::string getFileName(const std::string & vFileDirectory);
	//����һ���α��ļ�
	Common::SFileFrames* getFileFrames(int vIndex) { return &m_FilesData[vIndex]; }
	std::vector<std::vector<int>> getMultipleFramesIndex() { return m_MultipleFramesIndex; }
	void setDeformationStateFromFileName();
	CModelDeformationTransform*  getModelTransformStruct() { return m_ModelTransformStruct;}
	std::vector<Common::SFileFrames> searchFileFramesOnAnimation(const int vTheta, const int vPhi, const std::vector<double> & vForceFluctuationSequence);
	std::vector<Common::SFileFrames> searchFileFrameOnAttribute();
	
	double getKMatrixSumNumber()
	{
		double Sum = 0;
		if (!m_AllReallyLoadConnectedFem.empty())
			for(int i=0;i< m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].KLengths.size();i++)
				Sum+= m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].KLengths[i];
		return Sum;
	}
	std::vector<int> getKMatrixEachElementRelatedDataLength() { if (!m_AllReallyLoadConnectedFem.empty()) return m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].KLengths; }
	double getVeocitySumNumber() {if (!m_AllReallyLoadConnectedFem.empty()) return m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].Velocity.size();}
	double getInternalForceSumNuber() { if (!m_AllReallyLoadConnectedFem.empty()) return m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].InternalForces.size(); }

	void cleanSFileDataGroup(int vConnectionIndex, int vTimestep);

	void initMatchedFrameStruct(int vTreeSize);
	//���Ҽ�����׼
	void searchMatchedFrameSegment(std::vector<std::vector<glm::vec3>>& voMatchedFramesSequences, Common::SpKVFData& voSpKVData, std::vector<int> &vExtraForces, bool vIsFirstFrame);
	
	void searchMatchedDeformationFrames(std::vector<glm::vec3> & vUdeltaDeformation,std::vector<glm::vec3> & vFrameUDeformationData);

	void searchMatchedFrameSequences(std::vector<std::vector<int>> &voExtraForces);

	void searchMatchedOneTreeFrameSequences(std::vector<int> & voMatchedFrameSequenceIndex, Common::SpKVFData& voSpKVData, std::vector<int>&voExtraForces, int &voCurrentFrameIndex, int& vIsFirstFrame);

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
	inline int AbsError(int vFirstNumber, int vSecondNumber)
	{
		return abs(vFirstNumber - vSecondNumber);
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
	inline double GaussianFunction(double vVariable,double vSigma=1, double vMiu=0,double vTruncated=0.001)
	{
		double GaussianValue=(1 / (vSigma*sqrt(2 * Common::Pi))*exp(-0.5*pow((vVariable - vMiu) / vSigma, 2)));
		if (GaussianValue < vTruncated)
			GaussianValue = 0;
		return GaussianValue;
	}


private:

	void __getFileSeekDirOfEachBlock(const std::string& vFilePath, std::vector<long long>& vBlock);
	//·�������ÿ���ļ��ľ���·�����ϣ��磺D:\GraduationProject\Vega\models\8.10\test\positionthe0phi0force500,500,500.txt
	std::vector<std::string> m_FilePathList;
	//�α��ļ��ļ���
	std::vector<Common::SFileFrames> m_FilesData;
	std::vector<Common::SConnectedFemFiles> m_AllReallyLoadConnectedFem;
	//���Կ�����һ��obj��model������mesh���ϣ��Լ�group�鼯��
	CModelDeformationTransform* m_ModelTransformStruct;

	//��Ҫƥ���ĳЩ�����µ�Object����
	std::vector<std::vector<int>> m_CorrectDeformationIndex;

	//��һ�����ļ���SpKVF�Ķ�������4��9��14��19���ڶ���ֵ�ǵ�ǰ��������������
	std::vector<std::pair<int, std::vector<int>>> m_ForceSequence;
	//SpKVF�Ķ������ţ���ǰ������K����
	std::vector<std::pair<int, std::vector<std::vector<double>>* >> m_KMartixSequence;
	//SpKVF�Ķ������ţ���ǰ������v����
	std::vector<std::pair<int, std::vector<glm::vec3>* >> m_VelocitySequence;
	//SpKVF�Ķ������ţ���ǰ������f�ھ���
	std::vector<std::pair<int, std::vector<glm::vec3>* >> m_InternalForcesSequence;
	//�����ļ���SpKVF��������
	std::vector<int> m_reorderSpKVFSegmentIndexSequence;
	
	std::vector<std::vector<int>> m_MultipleFramesIndex;
	std::vector<Common::SpKVFData> m_TempSpKVFData;
	std::vector<int> m_CurrentFrameIndex;
	std::vector<int> m_Flag;


	int CurrentFrameIndex = 0;

};
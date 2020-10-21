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
	void readFramesDeformationData(std::vector<Common::SFileFrames>& vSearchFrames, int vSearchConnectionIndex);
	void readFramesDeformationDataBasedFilesIndex(std::vector<std::pair<int, int>>&vFilesAndFramesIndexSequence, std::vector<std::vector<glm::vec3>> &voMatchedFramesData);
	void readDeformationDataByMutileThread(Common::SFileFrames& vBaseFileFramesStruct, const std::string& vFilePath, int vSFileFramesIndex);
	void readDeformationDataInOneThread(Common::SFileData& vFileData, int vTid, const std::string& vFilePath, long long vBlockSize);
	void addSeekgOfEachFramesBlock(const std::string& vFilePath);
	std::vector<Common::SFileDataGroup> getConnectedFemMutileDeformation(int vConnectionIndex, int vTimestep);
	//�����ļ��ľ���·��ȡ�ļ�����,�磺positionthe0phi0force500,500,500
	std::string getFileName(const std::string & vFileDirectory);
	//����һ���α��ļ�
	Common::SFileFrames getFileFrames(int vIndex) { return m_FilesData[vIndex]; }
	void setDeformationStateFromFileName();
	CModelDeformationTransform*  getModelTransformStruct() { return m_ModelTransformStruct;}
	std::vector<Common::SFileFrames> searchFileFramesOnAnimation(const int vTheta, const int vPhi, const std::vector<int> & vForceFluctuationSequence);
	std::vector<Common::SFileFrames> searchFileFrameOnAttribute();
	std::vector<std::vector<glm::vec3>> objDeformation(std::pair<int, int> vForceDirection, std::vector<int> vForceFluctuationSequence);
	void cleanSFileDataGroup(int vConnectionIndex, int vTimestep);
	//���Ҽ�����׼
	void searchMatchedFrameSegment(std::vector<std::vector<glm::vec3>> &voMatchedFramesSequences);
	
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
};
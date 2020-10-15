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
#include "ModelDeformationTransform.h"

class CVegaFemFactory
{
public:

	CVegaFemFactory() = default;
	CVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile);
	~CVegaFemFactory() = default;

	void readKVFFileData(const std::string & vFile, Common::SFileFrames & vFileFrame);
	void readUdeformationData(const std::string & vFile, Common::SFileFrames &vFileFrame);
	void readFilePath4Directory(const std::string & vDirectoryName);
	void readFramesDeformationData(std::vector<Common::SFileFrames>& vSearchFrames, int vSearchConnectionIndex);
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
	
	void searchMatchedDeformationFrames();

private:

	void __getFileSeekDirOfEachBlock(const std::string& vFilePath, std::vector<long long>& vBlock);
	//·�������ÿ���ļ��ľ���·�����ϣ��磺D:\GraduationProject\Vega\models\8.10\test\positionthe0phi0force500,500,500.txt
	std::vector<std::string> m_FilePathList;
	//�α��ļ��ļ���
	std::vector<Common::SFileFrames> m_FilesData;
	std::vector<Common::SConnectedFemFiles> m_AllReallyLoadConnectedFem;
	//���Կ�����һ��obj��model������mesh���ϣ��Լ�group�鼯��
	CModelDeformationTransform* m_ModelTransformStruct;
};
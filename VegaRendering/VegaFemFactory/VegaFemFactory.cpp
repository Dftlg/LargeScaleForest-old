#include "VegaFemFactory.h"

CVegaFemFactory::CVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile)
{
	//m_FilesData中每个对象的文件名和句对路径加上
	readFilePath4Directory(vDirectoryName);
	//根据上面解析的文件名将m_FilesData每个对象的角度和波动序列加上
	setDeformationStateFromFileName();
	//可以看作是一个obj的model对象，有mesh集合，以及group组集合，
	m_ModelTransformStruct = new CModelDeformationTransform(vMutilVerticesBaseFile);
}

//将文件夹下所有文件创建多个SFileFrames文件对象，但并未加载数据，只是将文件对应的名字和绝对路径加上
void CVegaFemFactory::readFilePath4Directory(const std::string & vDirectoryName)
{
	intptr_t  hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(vDirectoryName).append("\\*.txt").c_str(), &fileinfo)) != -1)
	{
		do
		{
			m_FilePathList.push_back(p.assign(vDirectoryName).append("\\").append(fileinfo.name));

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	assert(m_FilePathList.size() > 0);

	for (auto i = 0; i < m_FilePathList.size(); i++)
	{
		m_FilesData.push_back(Common::SFileFrames(getFileName(m_FilePathList[i]), m_FilePathList[i]));
	}
	std::cout << "Load Multiple SFileFrames File:" << m_FilePathList.size() << std::endl;

}


//对于搜索出的SFileFrames加载数据
void CVegaFemFactory::readFramesDeformationData(std::vector<Common::SFileFrames>& vSearchFrames, int vSearchConnectionIndex)
{
	Common::SConnectedFemFiles tempConnectedFile(vSearchConnectionIndex);
	//记录所给角度下的位移需要几个位移文件来插值
	int counterConnectFileNumber = 0;
	//搜索到的指定角度下相关的位移文件集合
	time_t current = time(NULL);
	for (auto searchindex = 0; searchindex < vSearchFrames.size(); searchindex++)
	{
		//for循环路径下所有的位移文件
		for (auto fileIndex = 0; fileIndex < m_FilesData.size(); fileIndex++)
		{
			if (vSearchFrames[searchindex].FileName == m_FilesData[fileIndex].FileName)
			{
				//readDeformationDataByMutileThread(m_FilesData[fileIndex], m_FilesData[fileIndex].FilePath, fileIndex);
				
				readKVFFileData(m_FilesData[fileIndex].FilePath, m_FilesData[fileIndex]);
				int timeStepCount = 1;
				std::ifstream positionFile(m_FilesData[fileIndex].FilePath);
				std::string lineString;
				char s[4096];
				double position[3];
				if (!positionFile.is_open())
				{
					std::cout << "Error: could not open vertex file" << m_FilesData[fileIndex].FilePath << std::endl;
				}
				int Frameindex = 0;
				int stop = 0;
				while (stop==1)
				{
					getline(positionFile, lineString);
					//getline(positionFile, lineString);
					sprintf(s, "Position%04d", timeStepCount);
					std::istringstream sin(lineString);
					std::string str;
					sin >> str;//Position%04d后面有空格
					//一帧
					if (str == s)
					{
						std::string VertexSizeStr;
						getline(positionFile, VertexSizeStr);
						int VertexSize = atoi(VertexSizeStr.c_str());

						getline(positionFile, lineString);
						std::istringstream dataset(lineString);

						//一个位移文件数据中的一帧顶点位移
						Common::SFileData tempFileData(Frameindex);
						for (int j = 0; j < VertexSize; j++)
						{
							dataset >> position[0] >> position[1] >> position[2];
							tempFileData.BaseFileDeformations.push_back(glm::vec3(position[0], position[1], position[2]));
						}
						timeStepCount++;
						Frameindex++;
						m_FilesData[fileIndex].Frames.push_back(tempFileData);
					}
					stop++;
				}
				m_FilesData[fileIndex].isLoadDataSet = true;
				tempConnectedFile.FemDataset.push_back(&m_FilesData[fileIndex]);
				counterConnectFileNumber++;
			}
		}
	}
	time_t last = time(NULL);
	std::cout<<"time is"<<last-current<<std::endl;
	if (counterConnectFileNumber == 1) tempConnectedFile.Type = Common::EFileFramesType::OneRelatedFile;
	else if (counterConnectFileNumber == 2) tempConnectedFile.Type = Common::EFileFramesType::TwoRelatedFiles;
	else if (counterConnectFileNumber == 4) tempConnectedFile.Type = Common::EFileFramesType::FourRelatedFiles;

	m_AllReallyLoadConnectedFem.push_back(tempConnectedFile);

	std::cout << "Finish Load Search FileData To ConnectedFileFrames" << std::endl;
}

//read frames deformation based files index sequences,return matched frames data
void CVegaFemFactory::readFramesDeformationDataBasedFilesIndex(std::vector<std::pair<int, int>>&vFilesAndFramesIndexSequence, std::vector<std::vector<glm::vec3>> &voMatchedFramesData)
{
	std::vector<int>tempIndexSequece;
	for (int i = 0; i < vFilesAndFramesIndexSequence.size(); i++)
	{
		tempIndexSequece.push_back(vFilesAndFramesIndexSequence[i].first);
	}
	sort(tempIndexSequece.begin(), tempIndexSequece.end());
	tempIndexSequece.erase(std::unique(tempIndexSequece.begin(), tempIndexSequece.end()), tempIndexSequece.end());
	Common::SConnectedFemFiles tempConnectedFile(0);
	for (int i = 0; i < tempIndexSequece.size(); i++)
	{
		time_t current = time(NULL);
		int timeStepCount = 1;
		std::ifstream positionFile(m_FilesData[tempIndexSequece[i]].FilePath);
		std::string lineString;
		char s[4096];
		double position[3];
		if (!positionFile.is_open())
		{
			std::cout << "Error: could not open vertex file" << m_FilesData[tempIndexSequece[i]].FilePath << std::endl;
		}
		int Frameindex = 0;
		int stop = 0;
		while (getline(positionFile, lineString))
		{
			sprintf(s, "Position%04d", timeStepCount);
			std::istringstream sin(lineString);
			std::string str;
			sin >> str;//Position%04d后面有空格
			//一帧
			if (str == s)
			{
				std::string VertexSizeStr;
				getline(positionFile, VertexSizeStr);
				int VertexSize = atoi(VertexSizeStr.c_str());

				getline(positionFile, lineString);
				std::istringstream dataset(lineString);

				//一个位移文件数据中的一帧顶点位移
				Common::SFileData tempFileData(Frameindex);
				for (int j = 0; j < VertexSize; j++)
				{
					dataset >> position[0] >> position[1] >> position[2];
					tempFileData.BaseFileDeformations.push_back(glm::vec3(position[0], position[1], position[2]));
				}
				timeStepCount++;
				Frameindex++;
				m_FilesData[tempIndexSequece[i]].Frames.push_back(tempFileData);
			}
		}
		m_FilesData[tempIndexSequece[i]].isLoadDataSet = true;
		tempConnectedFile.FemDataset.push_back(&m_FilesData[tempIndexSequece[i]]);
		m_AllReallyLoadConnectedFem[tempIndexSequece[i]] = tempConnectedFile;
		time_t last = time(NULL);
		std::cout << "the time of load " << tempIndexSequece[i] << " is" << last - current << std::endl;
	}
	for (int i = 0; i < vFilesAndFramesIndexSequence.size(); i++)
	{
		voMatchedFramesData.push_back(m_AllReallyLoadConnectedFem[vFilesAndFramesIndexSequence[i].first].FemDataset[0]->Frames[vFilesAndFramesIndexSequence[i].second-4].BaseFileDeformations);
		voMatchedFramesData.push_back(m_AllReallyLoadConnectedFem[vFilesAndFramesIndexSequence[i].first].FemDataset[0]->Frames[vFilesAndFramesIndexSequence[i].second-3].BaseFileDeformations);
		voMatchedFramesData.push_back(m_AllReallyLoadConnectedFem[vFilesAndFramesIndexSequence[i].first].FemDataset[0]->Frames[vFilesAndFramesIndexSequence[i].second-2].BaseFileDeformations);
		voMatchedFramesData.push_back(m_AllReallyLoadConnectedFem[vFilesAndFramesIndexSequence[i].first].FemDataset[0]->Frames[vFilesAndFramesIndexSequence[i].second-1].BaseFileDeformations);
		voMatchedFramesData.push_back(m_AllReallyLoadConnectedFem[vFilesAndFramesIndexSequence[i].first].FemDataset[0]->Frames[vFilesAndFramesIndexSequence[i].second].BaseFileDeformations);
	}
}

void CVegaFemFactory::readKVFFileData(const std::string & vFile, Common::SFileFrames & vFileFrame)
{
	const size_t last_slash_idx = vFile.rfind('.txt');
	std::string FramesKVFFileName = vFile.substr(0, last_slash_idx - 3);
	FramesKVFFileName = FramesKVFFileName + ".spkvf";
	std::ifstream KVFFile(FramesKVFFileName);
	std::string lineString;
	if (!KVFFile.is_open())
	{
		std::cout << "Error: could not open vertex file" << FramesKVFFileName << std::endl;
		return;
	}
	
	double position[3];
	while (getline(KVFFile, lineString))
	{
		getline(KVFFile, lineString);
		int ElementNumber = atoi(lineString.c_str()) * 8 * 3;
		Common::SpKVFData tempKVFData;
		getline(KVFFile, lineString);
		if (lineString == "FrameIndex")
		{
			getline(KVFFile, lineString);
			tempKVFData.FrameIndex = atoi(lineString.c_str());
		}
		//getline(KVFFile, lineString);
		getline(KVFFile, lineString);
		if (lineString == "Force")
		{
			getline(KVFFile, lineString);
			std::istringstream ForceSet(lineString);
			std::vector<int> tempForces;
			for (auto k = 0; k < Common::ForcesSampling; k++)
			{
				double tempKnumber;
				ForceSet >> tempKnumber;
				tempForces.push_back(tempKnumber);
				//初始化存在错误
				//tempKNumbers.push_back(tempKnumber);
			}
			tempKVFData.Forces = tempForces;
		}
		getline(KVFFile, lineString);
		for (auto i = 0; i < ElementNumber; i++)
		{		
			getline(KVFFile, lineString);
			std::istringstream DataSet(lineString);
			int KmatrixOneLineNumber;
			DataSet >> KmatrixOneLineNumber;
			tempKVFData.KLengths.push_back(KmatrixOneLineNumber);
			std::vector<double> tempKNumbers;
			for (auto k = 0; k < KmatrixOneLineNumber; k++)
			{
				double tempKnumber;
				DataSet >> tempKnumber;
				//初始化存在错误
				tempKNumbers.push_back(tempKnumber);
			}
			tempKVFData.Kmatrix.push_back(tempKNumbers);
		}
		getline(KVFFile, lineString);
		if (lineString == "internalForces")
		{
			getline(KVFFile, lineString);
			int internalForceNumbers = atoi(lineString.c_str());
			for (auto i = 0; i < internalForceNumbers; i++)
			{
				getline(KVFFile, lineString);
				std::istringstream InterFace(lineString);
				double temp;
				InterFace >> temp;
				for (int j = 0; j < 3; j++)
				{
					InterFace >> position[0] >> position[1] >> position[2];				
				}
				tempKVFData.InternalForces.push_back(glm::vec3(position[0], position[1], position[2]));
			}
		}
		getline(KVFFile, lineString);
		if (lineString == "velocity")
		{
			getline(KVFFile, lineString);
			int velocityNumbers = atoi(lineString.c_str());
			for (auto i = 0; i < velocityNumbers; i++)
			{
				getline(KVFFile, lineString);
				std::istringstream Velocity(lineString);
				double temp;
				Velocity >> temp;
				for (int j = 0; j < 3; j++)
				{
					Velocity >> position[0] >> position[1] >> position[2];
				}
				tempKVFData.Velocity.push_back(glm::vec3(position[0], position[1], position[2]));
			}
		}
		vFileFrame.KVFFrameDatas.push_back(tempKVFData);
	}
	
}

//多线程读取一个文件
//第二个参数已经加载的结构体的第几个FileFrames
void CVegaFemFactory::readDeformationDataByMutileThread(Common::SFileFrames & vBaseFileFramesStruct, const std::string & vFilePath, int vSFileFramesIndex)
{
	std::vector<long long> BlockSizes;
	__getFileSeekDirOfEachBlock(vFilePath, BlockSizes);

	std::vector<Common::SFileData> Frames;
	for (int i = 0; i < Common::readFrames; i++)
	{
		Common::SFileData tempFileData(i);
		Frames.push_back(tempFileData);
	}

	//int i = 2;
	//readDeformationDataInOneThread(Frames[i], i, vFilePath, BlockSize);
	std::vector<std::thread> ReadFrameThreads;
	for (int i = 0; i < Common::readFrames; i++)
	{
		Common::SFileData& temp = Frames[i];
		std::thread th(&CVegaFemFactory::readDeformationDataInOneThread, this, std::ref(Frames[i]), i, vFilePath, BlockSizes[i]);
		ReadFrameThreads.emplace_back(std::move(th));
	}

	auto it = ReadFrameThreads.begin();
	for (; it != ReadFrameThreads.end(); ++it)
		(*it).join();

	std::this_thread::sleep_for(std::chrono::seconds(3));
	vBaseFileFramesStruct.Frames = Frames;
}

void CVegaFemFactory::readDeformationDataInOneThread(Common::SFileData & vFileData, int vTid, const std::string & vFilePath, long long vBlockSize)
{
	std::ifstream positionFile(vFilePath.c_str(), std::ios::in);
	if (!positionFile.good())
	{
		std::cout << "Thread :" << vTid << "failed to Open" << std::endl;
		return;
	}
	std::cout << "vTid" << vTid * vBlockSize << std::endl;
	positionFile.seekg(vBlockSize);
	char PositionIndex[4096];
	double position[3];
	std::string lineString;

	sprintf(PositionIndex, "Position%04d", vTid + 1);
	getline(positionFile, lineString);
	std::istringstream sin(lineString);
	std::string FilePositonIndex;
	sin >> FilePositonIndex;
	if (FilePositonIndex == PositionIndex)
	{
		std::cout << FilePositonIndex << std::endl;
		std::string VertexSizeStr;
		getline(positionFile, VertexSizeStr);
		int VertexSize = atoi(VertexSizeStr.c_str());

		getline(positionFile, lineString);
		std::istringstream Dataset(lineString);

		for (int j = 0; j < VertexSize; j++)
		{
			Dataset >> position[0] >> position[1] >> position[2];
			vFileData.BaseFileDeformations.push_back(glm::vec3(position[0], position[1], position[2]));
		}
	}
}

void CVegaFemFactory::addSeekgOfEachFramesBlock(const std::string & vFilePath)
{
	const size_t last_slash_idx = vFilePath.rfind('.txt');
	std::string FramesBlockFileName = vFilePath.substr(0, last_slash_idx - 3);
	FramesBlockFileName = FramesBlockFileName + "Block.block";
	std::ifstream positionFile(vFilePath.c_str(), std::ios::in);
	if (!positionFile.good())
	{
		std::cout << "failed to Open File" << vFilePath << "Can't get File Para" << std::endl;
		return;
	}
	std::vector<long long>tempBlockSizes;
	tempBlockSizes.push_back(0);
	std::string lineString;
	while (getline(positionFile, lineString))
	{
		getline(positionFile, lineString);
		getline(positionFile, lineString);
		long long BlockSize = positionFile.tellg();
		tempBlockSizes.push_back(BlockSize);
	}
	positionFile.close();

	std::ofstream writePositionFile(FramesBlockFileName, std::ios::out);
	for (auto i = 0; i < tempBlockSizes.size(); i++)
	{
		writePositionFile << tempBlockSizes[i] << " ";
	}
	writePositionFile << std::endl;
	writePositionFile.close();
}

//对于已经创建的多个SFileFrames对象，根据每个对象的文件名解析出其对应的两个角度以及力的波动序列，进行对象的填充，此时还是没有读入每帧的位移数据
void CVegaFemFactory::setDeformationStateFromFileName()
{
	int thetaPos;
	int phiPos;
	int forcePos;
	for (auto i = 0; i < m_FilesData.size(); i++)
	{
		std::vector<std::string> ForceSequence;
		thetaPos = m_FilesData[i].FileName.find("the");
		phiPos = m_FilesData[i].FileName.find("phi");
		forcePos = m_FilesData[i].FileName.find("force");
		m_FilesData[i].Theta = std::stoi(m_FilesData[i].FileName.substr(thetaPos + 3, phiPos - thetaPos - 3));
		m_FilesData[i].Phi = std::stoi(m_FilesData[i].FileName.substr(phiPos + 3, forcePos - phiPos - 3));
		//std::string tempsequence="500,500,500"
		//std::string tempsequence = m_FilesData[i].FileIndex.substr(forcePos + 5);
		//std::vector<std::string> ForceSequence={500,500,500}
		boost::split(ForceSequence, m_FilesData[i].FileName.substr(forcePos + 5), boost::is_any_of(","), boost::token_compress_off);
		std::vector<std::string>::iterator it;
		for (it = ForceSequence.begin(); it != ForceSequence.end(); ++it)
		{
			m_FilesData[i].ForceFluctuationSequence.push_back(std::stoi(*it));
		}
	}
}

std::string CVegaFemFactory::getFileName(const std::string & vFileDirectory)
{
	int firstPos = vFileDirectory.find_last_of("\\");
	int secondPos = vFileDirectory.find_last_of(".");
	std::string fileName = vFileDirectory.substr(firstPos + 1, secondPos - firstPos - 1);
	return fileName;
}

//将模型的原始形变数据扩大。
//vConnectionIndex代表了第i棵树，vTimestep代表了第几帧
std::vector<Common::SFileDataGroup> CVegaFemFactory::getConnectedFemMutileDeformation(int vConnectionIndex, int vTimestep)
{
	//搜索所有已经存储的相关Connection项,得出每一棵树位移所需要的相关位移文件
	for (int connectIndex = 0; connectIndex < m_AllReallyLoadConnectedFem.size(); connectIndex++)
	{
		//找到了第vConnectionIndex所需要的插值位移文件
		if (vConnectionIndex == m_AllReallyLoadConnectedFem[connectIndex].ConnectedIndex)
		{
			//这里就暂定插值文件就是本身，即不需要通过其它文件来插值
			if (m_AllReallyLoadConnectedFem[connectIndex].Type == Common::EFileFramesType::OneRelatedFile)
			{
				//对一个位移文件中的所有帧数进行循环
				for (int time = 0; time < (m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames.size(); time++)
				{
					//找到当前要绘制的帧数
					if ((m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time].FrameIndex == vTimestep)
					{
						//返回模型的增量
						m_ModelTransformStruct->ConvertVertex2mutileVerteices((m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time]);

						return (m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time].FileDeformation;

					}
				}
				std::cout << "Finish Load Multile Vertex" << std::endl;
			}
		}


	}
}

void CVegaFemFactory::cleanSFileDataGroup(int vConnectionIndex, int vTimestep)
{
	for (int connectIndex = 0; connectIndex < m_AllReallyLoadConnectedFem.size(); connectIndex++)
	{
		if (vConnectionIndex == m_AllReallyLoadConnectedFem[connectIndex].ConnectedIndex)
		{
			for (int time = 0; time < (m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames.size(); time++)
			{
				if ((m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time].FrameIndex == vTimestep)
				{
					(m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time].FileDeformation.clear();
				}
			}
		}
	}
}

//以5帧为单位进行帧段的查找匹配操作，组成一个匹配好的序列
void CVegaFemFactory::searchMatchedFrameSegment(std::vector<std::vector<glm::vec3>>& voMatchedFramesSequences)
{
	std::vector<int>ExtraForces = GenerateSamplingForce(60,135, 1, 0, 0);
	Common::SpKVFData tempSpKVData;
	for (int i = 0; i < 5; i++)
	{
		tempSpKVData.Forces.push_back(ExtraForces[i]);
	}
	//int sampleFrameNum = m_AllReallyLoadConnectedFem[0].FemDataset[0]->Frames.size();
	int sampleFrameNum = 60;
	std::vector<int> FrameIndexSequence;
	
	std::vector<std::pair<int, std::vector<int>>> tempForceSequence;	
	std::vector<int> reorderSegmentIndexSequence;
	for (int i = 0; i < m_AllReallyLoadConnectedFem.size(); i++)
	{
		for (int k = 0; k < m_AllReallyLoadConnectedFem[i].FemDataset[0]->KVFFrameDatas.size(); k++)
		{
			std::vector<int> tempExternalForces = m_AllReallyLoadConnectedFem[i].FemDataset[0]->KVFFrameDatas[k].Forces;
			reorderSegmentIndexSequence.push_back(m_AllReallyLoadConnectedFem[i].FemDataset[0]->KVFFrameDatas[k].FrameIndex + sampleFrameNum * i);
			tempForceSequence.push_back(std::make_pair(m_AllReallyLoadConnectedFem[i].FemDataset[0]->KVFFrameDatas[k].FrameIndex+sampleFrameNum*i, tempExternalForces));
		}
	}

	int frameNum = 12;
	std::vector<int> rangeTogether;
	while (FrameIndexSequence.size()<frameNum)
	{
		std::vector<std::pair<int, double>>tempForceErrorSequence;
		//compare forces
		for (int i = 0; i < tempForceSequence.size(); i++)
		{
			int forceError = 0;
			int currentForceIndex = FrameIndexSequence.size();
			/*for (int j = 0; j < 5; j++)
			{
				forceError += (tempSpKVData.Forces[j] - tempForceSequence[i].second[j])*(tempSpKVData.Forces[j] - tempForceSequence[i].second[j]);
			}*/

			forceError += (tempSpKVData.Forces[0] - tempForceSequence[i].second[0])*(tempSpKVData.Forces[0] - tempForceSequence[i].second[0]);
			forceError += (tempSpKVData.Forces[2] - tempForceSequence[i].second[2])*(tempSpKVData.Forces[2] - tempForceSequence[i].second[2]);
			forceError += (tempSpKVData.Forces[4] - tempForceSequence[i].second[4])*(tempSpKVData.Forces[4] - tempForceSequence[i].second[4]);
			tempForceErrorSequence.push_back(std::make_pair(tempForceSequence[i].first, forceError));
		}

		//initial tempSpKVData just based on Forces when find the first section
		if (FrameIndexSequence.size()==0)
		{
			std::vector<std::pair<int, double>>tempSortedForceSequence = tempForceErrorSequence;
			sort(tempSortedForceSequence.begin(), tempSortedForceSequence.end(), [](const std::pair<int, int>&x, const std::pair<int, int>&y)->int {return x.second < y.second; });
			tempSpKVData = m_AllReallyLoadConnectedFem[tempForceErrorSequence[0].first / 60].FemDataset[0]->KVFFrameDatas[(tempForceErrorSequence[0].first % 60) / 5];
			std::vector<int>tempForces;
			for (int i = 5; i < 10; i++)
			{
				tempForces.push_back(ExtraForces[i]);
			}
			tempSpKVData.Forces = tempForces;
			FrameIndexSequence.push_back(tempSortedForceSequence[0].first);
			continue;
		}
		//k
		std::vector<std::pair<int, std::vector<std::vector<double>>>>tempKMartixSequence;
		std::vector<std::pair<int, double>> tempKErrorSequence;
		std::vector<std::vector<double>> tempKMartix;
		for (int i = 0; i < reorderSegmentIndexSequence.size(); i++)
		{
			tempKMartix = m_AllReallyLoadConnectedFem[reorderSegmentIndexSequence[i] / 60].FemDataset[0]->KVFFrameDatas[(reorderSegmentIndexSequence[i] % 60)/5].Kmatrix;
			tempKMartixSequence.push_back(std::make_pair(reorderSegmentIndexSequence[i], tempKMartix));
			tempKMartix.clear();
		}
		std::vector<std::vector<double>>().swap(tempKMartix);
		//compare K Martix
		for (int i = 0; i < tempKMartixSequence.size(); i++)
		{
			int count = 0;
			if (tempKMartixSequence[i].first % 60 == 4)
			{
				for (int k = 0; k < tempKMartixSequence[i].second.size(); k++)
				{
					for (int j = 0; j < tempKMartixSequence[i].second[k].size(); j++)
					{
						if (abs(tempSpKVData.Kmatrix[k][j]-0) < 10)
							count++;
					}
				}
				tempKErrorSequence.push_back(std::make_pair(tempKMartixSequence[i].first, count));
				continue;
			}
			for (int k = 0; k < tempKMartixSequence[i].second.size(); k++)
			{
				for (int j = 0; j < tempKMartixSequence[i].second[k].size(); j++)
				{
					//tempkEvaluation += (tempKMartixSequence[i].second[k][j] - tempSpKVData.Kmatrix[k][j])*(tempKMartixSequence[i].second[k][j] - tempSpKVData.Kmatrix[k][j]);
					//tempkEvaluation += abs(tempKMartixSequence[i].second[k][j]);
					if (abs(tempKMartixSequence[i-1].second[k][j] - tempSpKVData.Kmatrix[k][j]) < 10)
						count++;
				}
			}
			tempKErrorSequence.push_back(std::make_pair(tempKMartixSequence[i].first, count));
		}

		//velocity
		std::vector<std::pair<int, std::vector<glm::vec3>>>tempVelocitySequence;
		std::vector<std::pair<int, double>>tempVelocityErrorSequence;
		std::vector<glm::vec3>tempVelocity;
		for (int i = 0; i < reorderSegmentIndexSequence.size(); i++)
		{
			tempVelocity = m_AllReallyLoadConnectedFem[reorderSegmentIndexSequence[i] / 60].FemDataset[0]->KVFFrameDatas[(reorderSegmentIndexSequence[i] % 60) / 5].Velocity;
			tempVelocitySequence.push_back(std::make_pair(reorderSegmentIndexSequence[i], tempVelocity));
			tempVelocity.clear();
		}
		std::vector<glm::vec3>().swap(tempVelocity);
		//compare velocity
		double velocityErrorRange = 0.001;
		for (int i = 0; i < tempVelocitySequence.size(); i++)
		{
			double count = 0;
			if (tempVelocitySequence[i].first % 60 == 4)
			{
				for (int k = 0; k < tempVelocitySequence[i].second.size(); k++)
				{
					if (abs(tempSpKVData.Velocity[k].x) < velocityErrorRange && abs(tempSpKVData.Velocity[k].y) < velocityErrorRange && abs(tempSpKVData.Velocity[k].z) < velocityErrorRange)
						count++;
				}
				tempVelocityErrorSequence.push_back(std::make_pair(tempVelocitySequence[i].first, count));
				continue;
			}
			for (int k = 0; k < tempVelocitySequence[i].second.size(); k++)
			{
				if (abs(tempVelocitySequence[i-1].second[k].x - tempSpKVData.Velocity[k].x < velocityErrorRange)&& abs(tempVelocitySequence[i-1].second[k].y - tempSpKVData.Velocity[k].y) < velocityErrorRange && abs(tempVelocitySequence[i-1].second[k].z - tempSpKVData.Velocity[k].z < velocityErrorRange))
					count++;
			}
			tempVelocityErrorSequence.push_back(std::make_pair(tempVelocitySequence[i].first, count));
		}
		//sort(tempVelocityErrorSequence.begin(), tempVelocityErrorSequence.end(), [](const std::pair<int, int>&x, const std::pair<int, int>&y)->int {return x.second > y.second;	});

		//internalForces
		std::vector<std::pair<int, std::vector<glm::vec3>>>tempInternalForcesSequence;
		std::vector<std::pair<int, double>>tempInternalForcesErrorSequence;
		std::vector<glm::vec3>tempInternalForces;
		for (int i = 0; i < reorderSegmentIndexSequence.size(); i++)
		{
			tempInternalForces = m_AllReallyLoadConnectedFem[reorderSegmentIndexSequence[i] / 60].FemDataset[0]->KVFFrameDatas[(reorderSegmentIndexSequence[i] % 60) / 5].InternalForces;
			tempInternalForcesSequence.push_back(std::make_pair(reorderSegmentIndexSequence[i], tempInternalForces));
			tempInternalForces.clear();
		}
		std::vector<glm::vec3>().swap(tempInternalForces);
		//compare internalForces
		double internalForceErrorRange = 50;
		for (int i = 0; i < tempInternalForcesSequence.size(); i++)
		{
			double count = 0;
			if (tempInternalForcesSequence[i].first % 60 == 4)
			{
				for (int k = 0; k < tempInternalForcesSequence[i].second.size(); k++)
				{
					if (abs(tempSpKVData.InternalForces[k].x) < internalForceErrorRange && abs(tempSpKVData.InternalForces[k].y) < internalForceErrorRange &&abs(tempSpKVData.InternalForces[k].z) < internalForceErrorRange)
						count++;
				}
				tempInternalForcesErrorSequence.push_back(std::make_pair(tempInternalForcesSequence[i].first, count));
				continue;
			}
			for (int k = 0; k < tempInternalForcesSequence[i].second.size(); k++)
			{
				if (abs(tempInternalForcesSequence[i-1].second[k].x - tempSpKVData.InternalForces[k].x < internalForceErrorRange)&& abs(tempInternalForcesSequence[i-1].second[k].y - tempSpKVData.InternalForces[k].y < internalForceErrorRange)&& abs(tempInternalForcesSequence[i-1].second[k].z - tempSpKVData.InternalForces[k].z < internalForceErrorRange))
					count++;
			}
			tempInternalForcesErrorSequence.push_back(std::make_pair(tempInternalForcesSequence[i].first, count));
		}
		//sort(tempInternalForcesErrorSequence.begin(), tempInternalForcesErrorSequence.end(), [](const std::pair<int, int>&x, const std::pair<int, int>&y)->int {return x.second > y.second;	});

		std::vector <std::pair<int, double>>reorderForcesError = tempForceErrorSequence;
		std::vector <std::pair<int, double>>reorderKMartixError = tempKErrorSequence;
		std::vector <std::pair<int, double>>reorderVelocityError = tempVelocityErrorSequence;
		std::vector <std::pair<int, double>>reorderInternalForceError = tempInternalForcesErrorSequence;
		sort(reorderForcesError.begin(), reorderForcesError.end(), [](const std::pair<double, double>&x, const std::pair<double, double>&y)->double {return x.second < y.second;	});
		sort(reorderKMartixError.begin(), reorderKMartixError.end(), [](const std::pair<double, double>&x, const std::pair<double, double>&y)->double {return x.second > y.second;	});
		sort(reorderVelocityError.begin(), reorderVelocityError.end(), [](const std::pair<double, double>&x, const std::pair<double, double>&y)->double {return x.second > y.second;	});
		sort(reorderInternalForceError.begin(), reorderInternalForceError.end(), [](const std::pair<double, double>&x, const std::pair<double, double>&y)->double {return x.second > y.second;	});
		//find weight for forces、kMartix、V、internalForce
		double forcesErrorSum = 0;
		double kMartixErrorSum = 19917;
		double velocityErrorSum = 90;
		double internalForcesSum = 90;
		for (int i = 0; i < reorderForcesError.size(); i++)
		{
			if (reorderForcesError[i].first - reorderForcesError[0].first >200)
				break;
			forcesErrorSum += reorderForcesError[i].first;
		}
		int similarity = 0;
		for (int i = 0; i < 5; i++)
		{
			std::cout << reorderForcesError[i].first << " " << reorderKMartixError[i].first << std::endl;
			for (int k = 0; k < 5; k++)
			{
				if (reorderForcesError[i].first == reorderKMartixError[k].first)
				{
					//std::cout << i << "-" << k << std::endl;
					similarity++;
				}
			}
		}
		std::cout << similarity << std::endl;

		
	    for (int i = 0; i < reorderSegmentIndexSequence.size(); i++)
	    {
			//externalF weight
			if (tempForceErrorSequence[i].second - reorderForcesError[0].second > 200)
			{
				tempForceErrorSequence[i].second = 0;
			}
			else
			{
				tempForceErrorSequence[i].second = 1 - (tempForceErrorSequence[i].second / forcesErrorSum);
			}
			//KMartix weight
			tempKErrorSequence[i].second = tempKErrorSequence[i].second / kMartixErrorSum;
			//velocity weight
			tempVelocityErrorSequence[i].second = tempVelocityErrorSequence[i].second / velocityErrorSum;
			//internalForce weight
			tempInternalForcesErrorSequence[i].second = tempInternalForcesErrorSequence[i].second / internalForcesSum;
	    }
		/*std::vector <std::pair<int, double>> reorderForcesErrorT = tempForceErrorSequence;
		std::vector <std::pair<int, double>> reorderKMartixErrorT = tempKErrorSequence;
		sort(reorderForcesErrorT.begin(), reorderForcesErrorT.end(), [](const std::pair<double, double>&x, const std::pair<double, double>&y)->int {return x.second > y.second; });
		sort(reorderKMartixErrorT.begin(), reorderKMartixErrorT.end(), [](const std::pair<double, double>&x, const std::pair<double, double>&y)->int {return x.second > y.second; });*/

		//Select a weight between externalF、K、V、InternalF as the criterion for updating
		std::vector<std::pair<int, double>> allWeightsSumResults;
		double forcesWeight = 0.6;
		double kMartixWeight =0.2;
		double velocityWeight = 0.1;
		double internalForceWeight = 0.1;
		for (int i = 0; i < reorderSegmentIndexSequence.size(); i++)
		{
			double tempResult = 0;
			tempResult = (tempForceErrorSequence[i].second)*forcesWeight + (tempKErrorSequence[i].second)*kMartixWeight + (tempVelocityErrorSequence[i].second)*velocityWeight + (tempInternalForcesErrorSequence[i].second)*internalForceWeight;
			allWeightsSumResults.push_back(std::make_pair(reorderSegmentIndexSequence[i], tempResult));
		}

		sort(allWeightsSumResults.begin(), allWeightsSumResults.end(), [](const std::pair<double, double>&x, const std::pair<double, double>&y)->int {return x.second > y.second;});

		/*FrameIndexSequence.push_back(tempKErrorSequence[0].first);
		tempSpKVData = m_AllReallyLoadConnectedFem[tempKErrorSequence[0].first / 60].FemDataset[0]->KVFFrameDatas[(tempKErrorSequence[0].first % 60) / 5];*/
		FrameIndexSequence.push_back(allWeightsSumResults[0].first);
		std::cout << "seleted:" << allWeightsSumResults[0].first << std::endl;
		tempSpKVData = m_AllReallyLoadConnectedFem[allWeightsSumResults[0].first / 60].FemDataset[0]->KVFFrameDatas[(allWeightsSumResults[0].first % 60) / 5];
		tempSpKVData.Forces.clear();
		for (int i = 0; i < 5; i++)
		{
			tempSpKVData.Forces.push_back(ExtraForces[FrameIndexSequence.size() * 5 + i]);
		}
		std::vector<std::pair<int, double>>().swap(tempForceErrorSequence);
		std::vector<std::pair<int, double>>().swap(tempKErrorSequence);
		std::vector<std::pair<int, double>>().swap(tempVelocityErrorSequence);
		std::vector<std::pair<int, double>>().swap(tempInternalForcesErrorSequence);
	}


	for (int i = 0; i < FrameIndexSequence.size(); i++)
	{
		std::cout << FrameIndexSequence[i] / 60 << "--" << FrameIndexSequence[i] % 60 << " ";
	}
	std::cout << std::endl;
	std::vector<std::pair<int, int>>fileAndFrameIndexSequence;
	
	for (int i = 0; i < FrameIndexSequence.size(); i++)
	{	
		fileAndFrameIndexSequence.push_back(std::make_pair(FrameIndexSequence[i] / 60, FrameIndexSequence[i] % 60));
		/*voMatchedFramesSequences.push_back(m_AllReallyLoadConnectedFem[FrameIndexSequence[i] / 60].FemDataset[0]->Frames[(FrameIndexSequence[i] % 60)-4].BaseFileDeformations);
		voMatchedFramesSequences.push_back(m_AllReallyLoadConnectedFem[FrameIndexSequence[i] / 60].FemDataset[0]->Frames[(FrameIndexSequence[i] % 60)-3].BaseFileDeformations);
		voMatchedFramesSequences.push_back(m_AllReallyLoadConnectedFem[FrameIndexSequence[i] / 60].FemDataset[0]->Frames[(FrameIndexSequence[i] % 60)-2].BaseFileDeformations);
		voMatchedFramesSequences.push_back(m_AllReallyLoadConnectedFem[FrameIndexSequence[i] / 60].FemDataset[0]->Frames[(FrameIndexSequence[i] % 60)-1].BaseFileDeformations);
		voMatchedFramesSequences.push_back(m_AllReallyLoadConnectedFem[FrameIndexSequence[i] / 60].FemDataset[0]->Frames[(FrameIndexSequence[i] % 60)].BaseFileDeformations);*/
	}
	readFramesDeformationDataBasedFilesIndex(fileAndFrameIndexSequence, voMatchedFramesSequences);
}

void CVegaFemFactory::__getFileSeekDirOfEachBlock(const std::string & vFilePath, std::vector<long long>& vBlock)
{
	const size_t last_slash_idx = vFilePath.rfind('.txt');
	std::string FramesBlockFileName = vFilePath.substr(0, last_slash_idx - 3);
	FramesBlockFileName = FramesBlockFileName + "Block.block";

	std::ifstream positionFile(FramesBlockFileName.c_str(), std::ios::in);
	if (!positionFile.good())
	{
		std::cout << "failed to Open File" << vFilePath << "Can't get File Para" << std::endl;
		addSeekgOfEachFramesBlock(vFilePath);
		std::cout << "create Data Block" << std::endl;
	}
	positionFile.close();

	std::ifstream preparepositionFile(FramesBlockFileName.c_str(), std::ios::in);
	std::string lineString;
	getline(preparepositionFile, lineString);
	std::istringstream Dataset(lineString);
	for (int i = 0; i < Common::readFrames; i++)
	{
		long long tempData;
		Dataset >> tempData;
		vBlock.push_back(tempData);
	}
}

std::vector<std::vector<glm::vec3>> CVegaFemFactory::objDeformation(std::pair<int, int> vForceDirection, std::vector<int> vForceFluctuationSequence)
{
	std::vector<Common::SFileFrames> proximityFileFrames = searchFileFramesOnAnimation(vForceDirection.first, vForceDirection.second, vForceFluctuationSequence);
	std::vector<std::vector<glm::vec3>> u;
	for (auto frame : proximityFileFrames)
	{

		//u.push_back(frame.Frames[0].PositionsDeformation);
	}

	//if(proximityFileFrames.size()==4)
	//search DataSet
	//权重设置,双线性插值
	/*std::string FileIndex=*/
	//返回一个模型的位移再由四面体转换到物体顶点的所有位移量。
	return u;
}

//在已经加载FileIndex但尚未加载位移数据的SFileFrames所有搜索相关的SFileFrames并返回
//返回临近的多个方向数据索引标识
std::vector<Common::SFileFrames> CVegaFemFactory::searchFileFramesOnAnimation(const int vTheta, const int vPhi, const std::vector<int>& vForceFluctuationSequence)
{
	int ThetaIndex = vTheta / 30;
	int PhiIndex = vPhi / 30;
	int ThetaSecondIndex = ThetaIndex + 1;
	int PhiSecondIndex = PhiIndex + 1;
	if (vTheta < 0)
	{
		ThetaSecondIndex = ThetaIndex - 1;
	}
	if (vPhi < 0)
	{
		PhiSecondIndex = PhiIndex - 1;
	}
	std::vector<Common::SFileFrames> fileFrames;
	if (vTheta % 30 == 0 && vPhi % 30 == 0)
	{
		for (auto file : m_FilesData)
		{
			if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
		}
	}
	else if (vTheta % 30 == 0 && vPhi % 30 != 0)
	{
		for (auto file : m_FilesData)
		{
			if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
			else if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiSecondIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
		}
	}
	else if (vTheta % 30 != 0 && vPhi % 30 == 0)
	{
		for (auto file : m_FilesData)
		{
			if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
			else if (file.Theta == (ThetaSecondIndex * 30) && file.Phi == (PhiIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
		}
	}
	else
	{
		for (auto file : m_FilesData)
		{
			if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
			else if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiSecondIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
			else if (file.Theta == (ThetaSecondIndex * 30) && file.Phi == (PhiIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
			else if (file.Theta == (ThetaSecondIndex * 30) && file.Phi == (PhiSecondIndex * 30))
			{
				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
				{
					fileFrames.push_back(file);
				}
			}
		}
	}
	return fileFrames;
}

std::vector<Common::SFileFrames> CVegaFemFactory::searchFileFrameOnAttribute()
{
	std::vector<Common::SFileFrames> fileFrames;
	for (auto file : m_FilesData)
	{
		fileFrames.push_back(file);
	}
	return fileFrames;
}


#include <iostream>
#include <GL/glew.h>
#include "VegaFemFactory.h"

void writePointPosition2File(const std::string& vInputIndex,const std::string& vOutputIndex,const int& vPointIndex, CVegaFemFactory& vFem)
{
	std::ifstream inputFile(vInputIndex);
	if (!inputFile.is_open())
	{
		std::cout << "Can't open the input file!!!" << std::endl;
	}
	std::ofstream outFile;
	outFile.open(vOutputIndex, std::ios::in | std::ios::app);
	if (!outFile)
	{
		std::cout << "Can't open the output file!!!" << std::endl;
		return;
	}
	std::string lineString;
	int fileIndex, frameIndex;
	float x = 0, y = 0, z = 0;
	while (getline(inputFile, lineString))
	{
		std::istringstream lineData(lineString);
		lineData >> fileIndex >> frameIndex;
		x = vFem.getFileFrames(fileIndex)->Frames[frameIndex].BaseFileDeformations[vPointIndex].x;
		y = vFem.getFileFrames(fileIndex)->Frames[frameIndex].BaseFileDeformations[vPointIndex].y;
		z = vFem.getFileFrames(fileIndex)->Frames[frameIndex].BaseFileDeformations[vPointIndex].z;
		outFile <<x <<" " << y<<" " << z << std::endl;
	}
}

int main()
{
	CVegaFemFactory vFem("G:/GraduationProject/yellow_tree/deltaU", "../../models/yellow_tree/tree_last.obj", "../../models/yellow_tree/ObjectVertexIndex.txt");
	int numbercounter = 6;
	std::vector<double> b{ 1000,1,0,0 };
	std::vector<std::pair<int, int>> angle;
	std::vector<Common::SFileFrames> vtemp = vFem.searchFileFrameOnAttribute();
	for (int i = 0; i < numbercounter; i++)
	{
		std::vector<Common::SFileFrames> temp;
		temp.push_back(vtemp[i]);
		vFem.readFramesDeformationData(temp, i);
	}
	writePointPosition2File("G:/GraduationProject/yellow_tree/xyz/FileAndFrameIndex.txt","G:/GraduationProject/yellow_tree/xyz/Interpolation_1150_32464_deltaU.txt",32464,vFem);
	return 0;
}
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

void readDataFromVegFile(std::string vPath, std::vector<glm::vec3>&voElementVertices, std::vector<std::vector<int>>& voElementIndex)
{
	std::ifstream inFile(vPath);
	if (!inFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	bool flag=true, verticesFlag = false, cubicIndexFlag = false;
	std::string lineString;
	while (flag)
	{
		getline(inFile, lineString);
		if (lineString == "*VERTICES")
		{
			verticesFlag = true;
			getline(inFile, lineString);
		}
		while (verticesFlag)
		{
			getline(inFile, lineString);
			if (lineString == "*ELEMENTS")
			{
				verticesFlag = false;
				cubicIndexFlag = true;
				getline(inFile, lineString);
				getline(inFile, lineString);
				break;
			}	
			if (lineString != "")
			{
				std::istringstream tempString(lineString);
				glm::vec3 tempVertex;
				float temp;
				tempString >> temp >> tempVertex.x >> tempVertex.y >> tempVertex.z;
				voElementVertices.push_back(tempVertex);
			}
		}
		while (cubicIndexFlag)
		{
			getline(inFile, lineString);
			if (lineString != "")
			{
				std::istringstream tempString(lineString);
				std::vector<int>tempCubicIndex;
				int tempSingleIndex;
				tempString >> tempSingleIndex;
				for (int i = 0; i < 8; i++)
				{
					tempString >> tempSingleIndex;
					tempCubicIndex.push_back(tempSingleIndex);
				}
				voElementIndex.push_back(tempCubicIndex);
			}
			else
			{
				cubicIndexFlag = false;
				flag = false;
			}
		}
	}
	inFile.close();
}

void readDataFromObjFile(std::string vPath, std::vector<glm::vec3>&voObjVertices)
{
	std::ifstream inFile(vPath);
	if (!inFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	std::string lineString;

	while (getline(inFile, lineString))
	{
		std::istringstream tempString(lineString);
		std::string tempChar;
		tempString >> tempChar;
		if (tempChar == "v")
		{
			glm::vec3 tempVertex;
			tempString >> tempVertex.x >> tempVertex.y >> tempVertex.z;
			voObjVertices.push_back(tempVertex);
		}
		if (tempChar == "vt" || tempChar == "vn")
		{
			break;
		}
	}
	inFile.close();
}

void findCubicIndexForVertices(std::vector<glm::vec3>& vObjVertices, std::vector<glm::vec3>&vElementVertices, std::vector<std::vector<int>>& vElementIndex, std::vector<int>& voObjVerticesRelatedElementIndex)
{
	for (int i = 0; i < vObjVertices.size(); i++)
	{
		for (int k = 0; k < vElementIndex.size(); k++)
		{
			std::vector<float>tempX, tempY, tempZ;
			for (int j = 0; j < vElementIndex[0].size(); j++)
			{
				tempX.push_back(vElementVertices[vElementIndex[k][j] - 1].x);
				tempY.push_back(vElementVertices[vElementIndex[k][j] - 1].y);
				tempZ.push_back(vElementVertices[vElementIndex[k][j] - 1].z);
			}
			float minX = *min_element(tempX.begin(), tempX.end());
			float maxX = *max_element(tempX.begin(), tempX.end());
			float minY = *min_element(tempY.begin(), tempY.end());
			float maxY = *max_element(tempY.begin(), tempY.end());
			float minZ = *min_element(tempZ.begin(), tempZ.end());
			float maxZ = *max_element(tempZ.begin(), tempZ.end());
			if (vObjVertices[i].x>minX && vObjVertices[i].x < maxX && vObjVertices[i].y> minY && vObjVertices[i].y < maxY && vObjVertices[i].z > minZ && vObjVertices[i].z < maxZ)
			{
				voObjVerticesRelatedElementIndex.push_back(k + 1);
				k = vElementIndex.size();
			}
		}
	}
}

void writeFindIndex2File(std::string vPath, std::vector<int>& vObjVerticesRelatedElementIndex)
{
	std::ofstream outFile(vPath);
	if (!outFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	for (int i = 0; i <vObjVerticesRelatedElementIndex.size(); i++)
	{
		outFile << vObjVerticesRelatedElementIndex[i] << ",";
	}
	outFile.close();
}

int main()
{
	std::vector<glm::vec3> elementVertices;
	std::vector<glm::vec3>objVertices;
	std::vector<std::vector<int>> elementIndex;
	std::vector<int> objVerticesRelatedElementIndex;
	std::string vegFilePath = "../../models/8.10/1.veg";
	std::string objFilePath = "../../models/1.obj";
	std::string indexOutputPath = "../../models/test.txt";
	readDataFromVegFile(vegFilePath, elementVertices, elementIndex);
	readDataFromObjFile(objFilePath, objVertices);
	findCubicIndexForVertices(objVertices, elementVertices, elementIndex, objVerticesRelatedElementIndex);
	writeFindIndex2File(indexOutputPath, objVerticesRelatedElementIndex);
	system("pause");
}
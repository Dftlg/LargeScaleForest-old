#include "ModelDeformationTransform.h"
#include <chrono>

CModelDeformationTransform::CModelDeformationTransform(const std::string & vFileName)
{
	//定义一个改良版的model，该model可以直接读取obj模型
	SceneObjectDeformable *m_BaseFileStruct = new SceneObjectDeformable(vFileName.c_str());
	//再将读取的mesh返回到这个本类的model的mesh
	m_BaseFileMesh = m_BaseFileStruct->GetMesh();
	__VertexFaceRelated();
}

//对m_Groups中的数据进行填充
void CModelDeformationTransform::__VertexFaceRelated()
{
	m_VerticesNumber = m_BaseFileMesh->getNumVertices();
	//循环obj模型中的总group数量
	for (unsigned int i = 0; i < m_BaseFileMesh->getNumGroups(); i++)
	{
		m_Groups.push_back(BaseObjConstruct::SGroup(m_BaseFileMesh->getGroup(i).getName(), m_BaseFileMesh->getGroup(i).getNumFaces()));
		std::vector<int> tempVertexIndex;
		std::vector<int> tempGroupIndex;
		//循环每个group中每个face的数量
		for (unsigned int iFace = 0; iFace < m_BaseFileMesh->getGroup(i).getNumFaces(); iFace++)
		{
			ObjMesh::Face face = m_BaseFileMesh->getGroup(i).getFace(iFace);

			//循环每个face中每个顶点的个数，一个face三个顶点
			for (int j = 0; j < 3; j++)
			{
				tempVertexIndex.push_back(face.getVertex(j).getPositionIndex());
				tempGroupIndex.push_back(face.getVertex(j).getPositionIndex());
			}
			m_Groups[i].Faces.push_back(BaseObjConstruct::SFace(tempVertexIndex));
			tempVertexIndex.clear();
		}
		m_GroupsIndex.push_back(tempGroupIndex);
		tempGroupIndex.clear();
	}
	std::cout << "Finish Load Vega Vertex And Face Relationship" << std::endl;
}

//根据原始u来填充按面的顺序排序的顶点u
void CModelDeformationTransform::ConvertVertex2mutileVerteices(Common::SFileData &vBaseFileDeformation)
{
	if (m_VerticesNumber != vBaseFileDeformation.BaseFileDeformations.size()) std::cout << "the DataSet not equal to baseFile" << std::endl;

	//循环group
	for (unsigned int i = 0; i < m_BaseFileMesh->getNumGroups(); i++)
	{
		Common::SFileDataGroup tempDataGroup(i);
		//每个group中的所有面
		for (unsigned int faceid = 0; faceid < m_Groups[i].FaceSize; faceid++)
		{
			//每个面中的所有顶点
			for (unsigned int vertexindex = 0; vertexindex < 3; vertexindex++)
			{
				tempDataGroup.PositionsDeformation.push_back(vBaseFileDeformation.BaseFileDeformations[m_Groups[i].Faces[faceid].VertexIndex[vertexindex]]);
			}
		}
		//将一帧中所有group对应的按照面排列的u进行存储
		vBaseFileDeformation.FileDeformation.push_back(tempDataGroup);
	}
}

void CModelDeformationTransform::SaveDeformationVertexFromBaseModel(const double* u, const int vDeformationSize, std::string vSaveFileName, int vtimeStepCounter)
{
	if (!vSaveFileName.empty())
	{
		char s[4096];
		FILE * file = fopen(vSaveFileName.c_str(), "a");
		if (!file)
		{
			printf("Can't open output file: %s.\n", s);
		}
		else
		{
			sprintf(s, "Position%04d", vtimeStepCounter);
			fprintf(file, "%s \n", s);
			sprintf(s, "%d", vDeformationSize);
			fprintf(file, "%s \n", s);
			for (unsigned int i = 0; i < vDeformationSize; i++)
			{
				fprintf(file, "%.10lf %.10lf %.10lf ", u[3 * i + 0], u[3 * i + 1], u[3 * i + 2]);
			}
			fprintf(file, "\n");
		}
		fclose(file);
	}
}
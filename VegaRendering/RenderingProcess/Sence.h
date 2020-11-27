#pragma once
#include <string.h>
#include <stdio.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Mesh.h"
//#include "TreeInstanceMesh.h"
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include "common.h"
#include "ExtraTool.h"
#include "VegaFemFactory.h"
#include "ObjStruct.h"
#include "objMesh.h"
#include "sceneObject.h"
#include "objMeshRender.h"

class CSence
{
public:
	CSence() = default;
	CSence(const std::string& vModelPath, bool vGamma = false, bool vloadNormalizeModelAndFaceNormal = false) :gammaCorrection(vGamma) { __loadModel(vModelPath, vloadNormalizeModelAndFaceNormal); };
	CSence(const ObjMesh * vobjMesh, double * vRestVertexs) : m_Mesh(vobjMesh), m_RestVertexs(vRestVertexs) { __changeObjMeshStruct2Charptr(1); __loadModelFromMemory(); };
	CSence(const CMesh& vMesh);
	~CSence() = default;

	void Clear() { glDeleteBuffers(1, &m_DeltaUSSBO);   glDeleteBuffers(1, &m_UdeformationSSBO);};
	std::vector<std::vector<glm::vec3>>& getGroupDeformationData() { return m_EachFrameOfGroupData; };
	void SetParaMesh();
	void setGroupsIndex(CVegaFemFactory& vfemFactoryObject);
	void setVerticesNumber(CVegaFemFactory& vfemFactoryObject);
	void setAssimpVerticesNumber();
	void initSSBODeformationDeltaU(CVegaFemFactory & vFem, int vFileNumber);
	void setSSBO4UDeformationAndIndex(const CShader& vShader);
	void initSSBODeformationU();
	void initSSBOTreeRotationModel();
	void initSSBOTreeFileAndFrameIndex(const int vTreeNumber);
	void resetSSBO4UDeformation();

	void setSSBOUdeformationAndIndx4ShadowMapShader(const CShader& vShader);

	std::vector<std::vector<int>> getGroupsIndex() { return m_GroupsIndex; }
	std::vector<CMesh> getMeshes() { return m_Meshes; }
	void senceDraw(const CShader& vShader, std::vector<std::vector<glm::vec3>> deformationFrames);
	void setFileDirectiory(std::string vFileDirectory) { m_FileDirectory = vFileDirectory; }
	int getAssimpVerticesNumber() { return m_AssimpVerticesNumber; }
	void draw(const CShader& vShader);
	void setMeshRotation(std::vector<Common::SWindDirecetion>& vTreesWindDirection);
	glm::mat4* randomRotation(std::vector<Common::SWindDirecetion>& vTreesWindDirection);
	void setMeshGroupAndAssimpIndex();

	void UpdataSSBOMeshTreeAndFrameIndex(std::vector<std::pair<int,int>>& vTreeFileAndFrameIndex);
	/*void UpdataMeshTreeAndFrameIndex(std::vector<int>& vTreeFileIndex, std::vector<int>& vFrameIndex);*/
	bool gammaCorrection;

	//(1,0)
	inline float getRotationAngle(float vXDisplacement, float vZDisplacement)
	{
		float dotAngle = acos((vXDisplacement) / sqrt(vXDisplacement*vXDisplacement + vZDisplacement * vZDisplacement)) * 180 / Common::Pi;
		float crossAngle = asin(vZDisplacement / sqrt(vXDisplacement*vXDisplacement + vZDisplacement * vZDisplacement)) * 180 / Common::Pi;
		if (crossAngle > 0)
			return(360 - dotAngle);
		return dotAngle;
	}

private:
	void __changeObjMeshStruct2Charptr(int vOutputMaterials=1);
	void __loadModel(const std::string& vModelPath, bool vloadNormalizeModel);
	void __loadModelFromMemory();
	void __processNode(const aiNode* vNode, const aiScene* vScene,bool vSaveDeformationOrLoadData);
	CMesh __processMesh(const aiMesh* vMesh, const aiScene* vScene);
	void __processSaveDeformation(const aiMesh*vMesh, const aiScene*vScene);
	std::vector<Common::STexture> loadMaterialTextures(aiMaterial *vMat, aiTextureType vType, std::string vtypeName);
	unsigned int TextureFromFile(const char *vPath, const std::string &vDirectory, bool vGamma = false);

	const ObjMesh * m_Mesh;
	std::vector<CMesh> m_Meshes;
	double *m_RestVertexs;
	std::vector<Common::STexture> m_Textures;
	std::string m_FileDirectory;
	std::string m_Data;
	unsigned int m_DataSize;
	std::vector<std::vector<glm::vec3>> m_EachFrameOfGroupData;
	//���е�group����Ķ�������
	std::vector<std::vector<int>> m_GroupsIndex;

	CVegaFemFactory* m_VegaFactory;

	glm::vec4 * m_DeltaDeformationU;

	glm::vec4 * m_DeformationU;

	glm::ivec2 * m_TreeFileAndFrameIndex;

	glm::mat4 * m_TreeRotationMatrix;

	unsigned int m_DeltaUSSBO;
	unsigned int m_UdeformationSSBO;
	unsigned int m_TreeFileAndFrameSSBO;
	unsigned int m_TreeRotationModelSSBO;

	int m_FrameNums;
	int m_VertexNums;
	int m_FileNumber;

	int m_VerticesNumber;

	int m_AssimpVerticesNumber;
};

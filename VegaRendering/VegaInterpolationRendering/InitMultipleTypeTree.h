#pragma once
#include <iostream>
#include <GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include<algorithm>
#include "../RenderingProcess/Camera.h"
#include "Shader.h"
#include "Sence.h"
#include "Mesh.h"
#include "VegaFemFactory.h"
#include "../Common/WindFiled.h"
#include "LoadWindAndTreeConfig.h"

class CInitMultipleTypeTree
{
public:
    CInitMultipleTypeTree()=default;
    CInitMultipleTypeTree(int vTreeTypeNumber, int vAllTreeNumbers) ;
    void InitVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile, const std::string &vCorrectDeformationUVertexIndex,int vTypeTreeRelatedFileNumber);
    void InitWindAndTree(int vTreeNumber, const std::string & vFilePath);
    void InitSceneShadowShader(const char* vVertexPath, const char* vFragmentPath);
    void InitSceneDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath = nullptr);

    void InitTreeModel(const std::string& vModelPath, int vTreeTypeIndex);
	int getSumFaceVerticesBeforeEndMesh(const int& vTreeIndex);

    void InitMultipleExtraWindData(int vTreeTypeIndex);

    void InitFemFrameStruct(int vTreeTypeIndex);
    void InitShadowCubeMapPara(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT,  std::vector <glm::mat4>& vshadowTransforms,glm::vec3 * vlightVertices,glm::vec3 * vlightColors);
   

    void InitScenceShaderData(int vTreeTypeIndex);
    ~CInitMultipleTypeTree()=default;

    std::vector<CVegaFemFactory*> * getFemFactory() { return &m_MultipleTypeFem; };
    std::vector<CSence*> * getTreeModel() { return &m_MultipleTreeModel; };
    std::vector<std::vector<std::vector<int>>> * getExtraForces() { return &m_MultipleExtraForces; };
    std::vector<std::vector<std::vector<Common::SForceDirection>>> * getExtraDirection() { return &m_MultipleExtraDirections; };
    std::vector<std::vector<int>> *getTreesNumberSubjected2SameWind() { return &m_MultipleTreesNumberSubjected2SameWind; };
    std::vector<CShader*> * getScenceShadowShader() { return &m_MultipleSceneShadowShader; };
    std::vector<CShader*> * getScenceDepthShader() { return &m_MultipleSceneDepthShader; };
   

    CVegaFemFactory* getSpecificFemFactory(int vTreeTypeIndex) { return m_MultipleTypeFem[vTreeTypeIndex]; };
    CSence * getSpecificTreeModel(int vTreeTypeIndex) { return m_MultipleTreeModel[vTreeTypeIndex]; };
    std::vector<std::vector<int>> * getSpecificExtraForces(int vTreeTypeIndex) { return &m_MultipleExtraForces[vTreeTypeIndex]; };
    std::vector<std::vector<Common::SForceDirection>> * getSpecificExtraDirection(int vTreeTypeIndex) { return &m_MultipleExtraDirections[vTreeTypeIndex]; };
    std::vector<int> *getSpecificTreesNumberSubjected2SameWind(int vTreeTypeIndex) { return &m_MultipleTreesNumberSubjected2SameWind[vTreeTypeIndex]; };
    CShader * getSpecificScenceShadowShader(int vTreeTypeIndex) { return m_MultipleSceneShadowShader[vTreeTypeIndex]; };
    CShader * getSpecificScenceDepthShader(int vTreeTypeIndex) { return m_MultipleSceneDepthShader[vTreeTypeIndex]; };
    CLoadWindAndTreeConfig getSpecificLoadWindAndTree(int vTreeTypeIndex) { return m_MultipleTypeTree[vTreeTypeIndex];}

    std::vector<int> getTreeTypeIndex() { return m_TreeTypeIndex; };

private:

    void __GenerateTreesPosition() { m_AllTreesPosition = RandomTreePositionGenerate(m_AllTreesNumber); };

    int m_TreeTypeNumber;
    //each type tree have one 
    std::vector<CVegaFemFactory*> m_MultipleTypeFem;
    std::vector<CLoadWindAndTreeConfig> m_MultipleTypeTree;
    std::vector<int> m_MultipleEachTreeRelatedFileNumber;
    std::vector<int> m_MultipleEachTreeProductNumber;
    std::vector<CShader*> m_MultipleSceneShadowShader;
    std::vector<CShader*> m_MultipleSceneDepthShader;
    std::vector<CSence *> m_MultipleTreeModel;

    //第一个std::vector表示某棵树，第二个vector表示该棵树共有多少中不同的形变，第三个vector表示某一形变收到的外力
    std::vector<std::vector<std::vector<int>>> m_MultipleExtraForces;
    std::vector<std::vector<std::vector<Common::SForceDirection>>> m_MultipleExtraDirections;
    std::vector<std::vector<int>> m_MultipleTreesNumberSubjected2SameWind;

    std::vector<int> m_TreeTypeIndex;

    //ShadowCubePara
    float m_vNearPlane;
    float m_vFarPlane;
    int m_ShadowWidth;
    int m_ShadowHeight;
    glm::vec3 m_LightVertice;
    std::vector <glm::mat4> m_ShadowTransforms;

    glm::vec3 m_lightVertices[4];
    glm::vec3 m_lightColors[4];

    int m_AllTreesNumber;
    std::vector<int> m_EachTypeTreesPositonArray;
    std::vector<std::pair<double, double>> m_AllTreesPosition;
};


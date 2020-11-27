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
    CInitMultipleTypeTree(int vTreeTypeNumber) { m_TreeTypeNumber = vTreeTypeNumber; };
    void InitVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile, const std::string &vCorrectDeformationUVertexIndex,int vTypeTreeRelatedFileNumber);
    void InitWindAndTree(int vTreeNumber, const std::string & vFilePath);
    void InitSceneShadowShader(const char* vVertexPath, const char* vFragmentPath);
    void InitSceneDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath = nullptr);

    void InitTreeModel(const std::string& vModelPath, int vTreeTypeIndex);

    void InitMultipleExtraWindData(int vTreeTypeIndex);

    void InitFemFrameStruct(int vTreeTypeIndex);
    void InitShadowCubeMapPara(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT,  std::vector <glm::mat4>& vshadowTransforms,glm::vec3 * vlightVertices,glm::vec3 * vlightColors);

    void InitScenceShaderData(int vTreeTypeIndex);
    ~CInitMultipleTypeTree()=default;

    CVegaFemFactory* getFemFactory(int vTreeTypeIndex) { return &m_MultipleTypeFem[vTreeTypeIndex]; };
    CSence * getTreeModel(int vTreeTypeIndex) { return &m_MultipleTreeModel[vTreeTypeIndex]; };
    std::vector<std::vector<int>> * getExtraForces(int vTreeTypeIndex) { return &m_MultipleExtraForces[vTreeTypeIndex]; };
    std::vector<std::vector<Common::SForceDirection>> * getExtraDirection(int vTreeTypeIndex) { return &m_MultipleExtraDirections[vTreeTypeIndex]; };
    std::vector<int> *getTreesNumberSubjected2SameWind(int vTreeTypeIndex) { return &m_MultipleTreesNumberSubjected2SameWind[vTreeTypeIndex]; };

private:
    void __LoadFemData();
    int m_TreeTypeNumber;
    //each type tree have one 
    std::vector<CVegaFemFactory> m_MultipleTypeFem;
    std::vector<CLoadWindAndTreeConfig> m_MultipleTypeTree;
    std::vector<int> m_MultipleEachTreeRelatedFileNumber;
    std::vector<int> m_MultipleEachTreeProductNumber;
    std::vector<CShader> m_MultipleSceneShadowShader;
    std::vector<CShader> m_MultipleSceneDepthShader;
    std::vector<CSence> m_MultipleTreeModel;

    //第一个std::vector表示某棵树，第二个vector表示该棵树共有多少中不同的形变，第三个vector表示某一形变收到的外力
    std::vector<std::vector<std::vector<int>>> m_MultipleExtraForces;
    std::vector<std::vector<std::vector<Common::SForceDirection>>> m_MultipleExtraDirections;
    std::vector<std::vector<int>> m_MultipleTreesNumberSubjected2SameWind;

    //ShadowCubePara
    float m_vNearPlane;
    float m_vFarPlane;
    int m_ShadowWidth;
    int m_ShadowHeight;
    glm::vec3 m_LightVertice;
    std::vector <glm::mat4> m_ShadowTransforms;

    glm::vec3 m_lightVertices[4];
    glm::vec3 m_lightColors[4];
};


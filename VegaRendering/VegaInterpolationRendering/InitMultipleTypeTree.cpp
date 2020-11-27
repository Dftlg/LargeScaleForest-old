#include "InitMultipleTypeTree.h"

void CInitMultipleTypeTree::InitVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile, const std::string &vCorrectDeformationUVertexIndex, int vTypeTreeRelatedFileNumber)
{
    CVegaFemFactory* vFem=new CVegaFemFactory(vDirectoryName, vMutilVerticesBaseFile, vCorrectDeformationUVertexIndex);
    std::vector<Common::SFileFrames> vtemp = vFem->searchFileFrameOnAttribute();
    m_MultipleEachTreeRelatedFileNumber.push_back(vTypeTreeRelatedFileNumber);
    for (int i = 0; i < vTypeTreeRelatedFileNumber; i++)
    {
        std::vector<Common::SFileFrames> temp;
        temp.push_back(vtemp[i]);
        vFem->readFramesDeformationData(temp, i);
    }
    m_MultipleTypeFem.push_back(vFem);
}

void CInitMultipleTypeTree::InitWindAndTree(int vTreeNumber, const std::string & vFilePath)
{
    m_MultipleEachTreeProductNumber.push_back(vTreeNumber);

    CLoadWindAndTreeConfig windAndTreeConfig(vTreeNumber, vFilePath);
    m_MultipleTypeTree.push_back(windAndTreeConfig);
}

void CInitMultipleTypeTree::InitSceneShadowShader(const char* vVertexPath, const char* vFragmentPath)
{
    CShader ourSceneShadowShader(vVertexPath, vFragmentPath);
    m_MultipleSceneShadowShader.push_back(ourSceneShadowShader);
}
void CInitMultipleTypeTree::InitSceneDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath)
{
    CShader ourSceneDepthShader(vVertexPath, vFragmentPath, vGeometryPath);
    m_MultipleSceneDepthShader.push_back(ourSceneDepthShader);
}

void CInitMultipleTypeTree::InitTreeModel(const std::string& vModelPath,int vTreeTypeIndex)
{
    CSence ourModel(vModelPath);
    std::vector<float> SpecificRotation = m_MultipleTypeTree[vTreeTypeIndex].getMultipleRotationAngle();
    ourModel.setMeshRotation(SpecificRotation);

    ourModel.setGroupsIndex(*(m_MultipleTypeFem[vTreeTypeIndex]));
    ourModel.setVerticesNumber(*(m_MultipleTypeFem[vTreeTypeIndex]));
    ourModel.setMeshGroupAndAssimpIndex();
    ourModel.initSSBODeformationDeltaU(*(m_MultipleTypeFem[vTreeTypeIndex]), m_MultipleEachTreeRelatedFileNumber[vTreeTypeIndex]);
    ourModel.initSSBODeformationU();
    ourModel.initSSBOTreeFileAndFrameIndex(m_MultipleEachTreeProductNumber[vTreeTypeIndex]);
    ourModel.setSSBO4UDeformationAndIndex(m_MultipleSceneShadowShader[vTreeTypeIndex]);
    ourModel.setSSBOUdeformationAndIndx4ShadowMapShader(m_MultipleSceneDepthShader[vTreeTypeIndex]);
    m_MultipleTreeModel.push_back(ourModel);
}

void CInitMultipleTypeTree::InitMultipleExtraWindData(int vTreeTypeIndex)
{
    std::vector<std::vector<SWaveFunctionPara>> OneDirectionWindRelatedMultipleTree = m_MultipleTypeTree[vTreeTypeIndex].getMultipleTreeWindPara();
    std::vector<int> TreesNumberSubjected2SameWind = m_MultipleTypeTree[vTreeTypeIndex].getMultipleTreeDuplicateNumber();
    m_MultipleTreesNumberSubjected2SameWind.push_back(TreesNumberSubjected2SameWind);
    //只计算了树受到不同的风力，若相同则直接取用已有数据
    for (int i = 0; i < OneDirectionWindRelatedMultipleTree.size(); i++)
    {
        CWindField OnedirectionWind(Common::ProductFrameNumber, OneDirectionWindRelatedMultipleTree[i], 6000, Common::SForceDirection(0, 0), m_MultipleTypeTree[vTreeTypeIndex].getTreeRotationAngle()[i]);
        m_MultipleExtraForces[vTreeTypeIndex].push_back(OnedirectionWind.getDirectionWindForces());
        m_MultipleExtraDirections[vTreeTypeIndex].push_back(OnedirectionWind.getDirectionWindDirection());
    }
}

void CInitMultipleTypeTree::InitFemFrameStruct(int vTreeTypeIndex)
{
    m_MultipleTypeFem[vTreeTypeIndex]->initMatchedFrameStruct(m_MultipleExtraForces[vTreeTypeIndex].size());
    m_MultipleTypeFem[vTreeTypeIndex]->initKVFDataSearchRangeError();
}

void CInitMultipleTypeTree::InitShadowCubeMapPara(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT, std::vector <glm::mat4>& vshadowTransforms, glm::vec3 * vlightVertices, glm::vec3 * vlightColors)
{
    m_vNearPlane= vNearPlane;
    m_vFarPlane= vFarPlane;
    m_ShadowWidth= vSHADOW_WIDTH;
    m_ShadowHeight= vSHADOW_HEIGHT;
    m_LightVertice= vlightVertices[0];
    m_ShadowTransforms= vshadowTransforms;
    for (int i = 0; i < 4; i++)
    {
        m_lightVertices[i] = vlightVertices[i];
        m_lightColors[i] = vlightColors[i];
    }

}

void CInitMultipleTypeTree::InitScenceShaderData(int vTreeTypeIndex)
{
    //帧数
    int frameNums = m_MultipleTypeFem[vTreeTypeIndex]->getFileFrames(0)->Frames.size();
    //obj model vertices
    int vertexNums = m_MultipleTypeFem[vTreeTypeIndex]->getFileFrames(0)->Frames[0].BaseFileDeformations.size();
    m_MultipleSceneDepthShader[vTreeTypeIndex].use();
    m_MultipleSceneDepthShader[vTreeTypeIndex].setInt("frameNums", frameNums);
    m_MultipleSceneDepthShader[vTreeTypeIndex].setInt("vertexNums", vertexNums);
    m_MultipleSceneDepthShader[vTreeTypeIndex].setInt("assimpvertexNums", m_MultipleTreeModel[vTreeTypeIndex].getAssimpVerticesNumber());
    m_MultipleSceneShadowShader[vTreeTypeIndex].use();
    m_MultipleSceneShadowShader[vTreeTypeIndex].setInt("frameNums", frameNums);
    m_MultipleSceneShadowShader[vTreeTypeIndex].setInt("vertexNums", vertexNums);
    m_MultipleSceneShadowShader[vTreeTypeIndex].setInt("assimpvertexNums", m_MultipleTreeModel[vTreeTypeIndex].getAssimpVerticesNumber());

    //create depth cubemap transformation matrices and some value
    m_MultipleSceneDepthShader[vTreeTypeIndex].use();
    m_MultipleSceneDepthShader[vTreeTypeIndex].setFloat("far_plane", m_vFarPlane);
    m_MultipleSceneDepthShader[vTreeTypeIndex].setVec3("lightPos", m_LightVertice);
    for (unsigned int i = 0; i < 6; ++i)
    {
        m_MultipleSceneDepthShader[vTreeTypeIndex].setMat4("shadowMatrices[" + std::to_string(i) + "]", m_ShadowTransforms[i]);
    }

    //set light to fragment
    m_MultipleSceneShadowShader[vTreeTypeIndex].use();
    for (unsigned int i = 0; i < sizeof(m_lightVertices) / sizeof(m_lightVertices[0]); ++i)
    {
        glm::vec3 newPos = m_lightVertices[i];
        m_MultipleSceneShadowShader[vTreeTypeIndex].setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        m_MultipleSceneShadowShader[vTreeTypeIndex].setVec3("lightColors[" + std::to_string(i) + "]", m_lightColors[i]);
    }
    m_MultipleSceneShadowShader[vTreeTypeIndex].setFloat("metallic", 0.04);
    m_MultipleSceneShadowShader[vTreeTypeIndex].setFloat("roughness", 0.8);
    m_MultipleSceneShadowShader[vTreeTypeIndex].setFloat("ao", 1.0f);
    m_MultipleSceneShadowShader[vTreeTypeIndex].setInt("depthMap", 8);
    m_MultipleSceneShadowShader[vTreeTypeIndex].setFloat("far_plane", m_vFarPlane);
}

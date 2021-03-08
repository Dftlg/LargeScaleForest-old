#include "InitMultipleTypeTree.h"

CInitMultipleTypeTree::CInitMultipleTypeTree(int vTreeTypeNumber, int vAllTreeNumbers, bool vWindFieldType)
{
    m_TreeTypeNumber = vTreeTypeNumber;
    m_AllTreesNumber = vAllTreeNumbers;
    m_MultipleExtraForces.resize(vTreeTypeNumber);
    m_MultipleExtraDirections.resize(vTreeTypeNumber);
    m_EachTypeTreesPositionInSence.resize(vTreeTypeNumber);
    m_OneDirectionWindOrSpecificWindSource = vWindFieldType;
    __GenerateTreesPosition();
    m_EachTypeTreesPositonArray.push_back(0);
    int Sum = 0;
    for (int i = 0; i < vTreeTypeNumber; i++)
    {
        Sum += Common::TreesNumbers[i];
        m_EachTypeTreesPositonArray.push_back(Sum);
    }
}

CInitMultipleTypeTree::~CInitMultipleTypeTree()
{
    for (auto p : m_MultipleTypeFem)
        delete p;
    m_MultipleTypeFem.clear();
    for (auto p : m_MultipleSceneShadowShader)
        delete p;
    m_MultipleSceneShadowShader.clear();
    for (auto p : m_MultipleSceneDepthShader)
        delete p;
    m_MultipleSceneDepthShader.clear();
    for (auto p : m_MultipleTreeModel)
        delete p;
    m_MultipleTreeModel.clear();
}


void CInitMultipleTypeTree::__GenerateTreesPosition()
{
    if (m_OneDirectionWindOrSpecificWindSource == true)
        __GenerateRandomTreesPosition();
    else
        __GenerateStableTreesPosition();
}

void CInitMultipleTypeTree::InitVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile, const std::string &vCorrectDeformationUVertexIndex,const std::string& vKVFGroupConnectObjGroup, int vTypeTreeRelatedFileNumber)
{
    CVegaFemFactory* vFem=new CVegaFemFactory(vDirectoryName, vMutilVerticesBaseFile, vCorrectDeformationUVertexIndex, vKVFGroupConnectObjGroup);
    std::vector<Common::SFileFrames> vtemp = vFem->searchFileFrameOnAttribute();
    //////相关文件需要乘3
    m_MultipleEachTreeRelatedFileNumber.push_back(vTypeTreeRelatedFileNumber);
    for (int i = 0; i < vTypeTreeRelatedFileNumber*vFem->getModelGroupNumber(); i++)
    {
        std::vector<Common::SFileFrames> temp;
        temp.push_back(vtemp[i]);
        vFem->readFramesDeformationData(temp, i);
    }
    m_MultipleTypeFem.push_back(vFem);
}

//在这里写位置
void CInitMultipleTypeTree::InitWindAndTree(int vTreeNumber, const std::string & vFilePath)
{
    m_MultipleEachTreeProductNumber.push_back(vTreeNumber);

    CLoadWindAndTreeConfig windAndTreeConfig(vTreeNumber, vFilePath);
    m_MultipleTypeTree.push_back(windAndTreeConfig);
}

//void CInitMultipleTypeTree::InitASpecificWindSourceTreeRotation(int vTreeNumber, const std::string & vFilePath)
//{
//    if (m_OneDirectionWindOrSpecificWindSource = true)
//        std::cout << "This step use on A Specific Wind Source,The other function InitWindAndTree use on a one Direction wind" << std::endl;
//    else
//    {
//        m_MultipleEachTreeProductNumber.push_back(vTreeNumber);
//
//    }
//}

void CInitMultipleTypeTree::InitSceneShadowShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath)
{
    CShader * ourSceneShadowShader=new CShader (vVertexPath, vFragmentPath, vGeometryPath);
    m_MultipleSceneShadowShader.push_back(ourSceneShadowShader);
}
void CInitMultipleTypeTree::InitSceneDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath)
{
    CShader * ourSceneDepthShader=new CShader(vVertexPath, vFragmentPath, vGeometryPath);
    m_MultipleSceneDepthShader.push_back(ourSceneDepthShader);
}

void CInitMultipleTypeTree::InitWindSource(const char* vVertexPath, const char* vFragmentPath, const std::string & vModelPath)
{
    m_WindSourceShader= new CShader(vVertexPath, vFragmentPath);
    m_WindSourceObject = new CSence(vModelPath);
}

void CInitMultipleTypeTree::renderingWindSource(glm::mat4 vPerspective, glm::mat4 vView, int vFrameIndex)
{
    m_WindSourceShader->use();
    m_WindSourceShader->setMat4("projection", vPerspective);
    m_WindSourceShader->setMat4("view", vView);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 2, 0));
    model = glm::translate(model, m_ASpecificWindSource->getSpecificWindSourcePosition(vFrameIndex));
    model = glm::scale(model, glm::vec3(0.3, 0.3, 0.3));
    m_WindSourceShader->setMat4("model", model);
    m_WindSourceObject->draw(*m_WindSourceShader);
}

void CInitMultipleTypeTree::InitTreeModel(const std::string& vModelPath,int vTreeTypeIndex, bool vWindTypeDecideTreePosition)
{
    CSence* ourModel=new CSence(vModelPath);

    ourModel->setTreeNumber(m_MultipleEachTreeProductNumber[vTreeTypeIndex]);
    std::vector<float> SpecificRotation = m_MultipleTypeTree[vTreeTypeIndex].getMultipleRotationAngle();
    std::vector<std::pair<double, double>> tempTransFormation;
    for (int i = m_EachTypeTreesPositonArray[vTreeTypeIndex]; i < m_EachTypeTreesPositonArray[vTreeTypeIndex + 1]; i++)
        tempTransFormation.push_back(m_AllTreesPosition[i]);
    
    ourModel->setMeshRotation(SpecificRotation, tempTransFormation,Common::ScaleTree[vTreeTypeIndex],Common::TreesNumbers[vTreeTypeIndex], vWindTypeDecideTreePosition);
    m_EachTypeTreesPositionInSence[vTreeTypeIndex] = ourModel->getTreePositions();
    m_AllTreesPositionInSence.insert(m_AllTreesPositionInSence.end(), m_EachTypeTreesPositionInSence[vTreeTypeIndex].begin(), m_EachTypeTreesPositionInSence[vTreeTypeIndex].end());

    ourModel->setGroupsIndex(*(m_MultipleTypeFem[vTreeTypeIndex]));
    ourModel->setVerticesNumber(*(m_MultipleTypeFem[vTreeTypeIndex]));
    ourModel->setMeshGroupAndAssimpIndex();
   
    ourModel->initSSBODeformationDeltaU(*(m_MultipleTypeFem[vTreeTypeIndex]), m_MultipleEachTreeRelatedFileNumber[vTreeTypeIndex]);
    ourModel->initSSBODeformationU();
    ourModel->initSSBOTreeFileAndFrameIndex(m_MultipleEachTreeProductNumber[vTreeTypeIndex]);
    ourModel->setSSBO4GenBufferUDeformationAndIndex(*(m_MultipleSceneShadowShader[vTreeTypeIndex]), vTreeTypeIndex);
   // ourModel->setSSBO4UDeformationAndIndex(*(m_MultipleSceneShadowShader[vTreeTypeIndex]));
    //ourModel->setSSBOUdeformationAndIndx4ShadowMapShader(*(m_MultipleSceneDepthShader[vTreeTypeIndex]));
    m_TreeTypeIndex.push_back(vTreeTypeIndex);
    m_MultipleTreeModel.push_back(ourModel);
}

int CInitMultipleTypeTree::getSumFaceVerticesBeforeEndMesh(const int & vTreeIndex)
{
    int sumFaceVerticesBeforeEndMesh = 0;
    for (int i = 0; i < m_MultipleTreeModel[vTreeIndex]->getMeshes().size() - 1; i++)
    {
        sumFaceVerticesBeforeEndMesh += m_MultipleTreeModel[vTreeIndex]->getMeshes()[i].getVertices().size();
    }
    return sumFaceVerticesBeforeEndMesh;

}

//计算定向风场
void CInitMultipleTypeTree::InitMultipleExtraWindData(int vTreeTypeIndex)
{
    std::vector<std::vector<SWaveFunctionPara>> OneDirectionWindRelatedMultipleTree = m_MultipleTypeTree[vTreeTypeIndex].getMultipleTreeWindPara();
    std::vector<int> TreesNumberSubjected2SameWind = m_MultipleTypeTree[vTreeTypeIndex].getMultipleTreeDuplicateNumber();
    m_MultipleTreesNumberSubjected2SameWind.push_back(TreesNumberSubjected2SameWind);
    //只计算了树受到不同的风力，若相同则直接取用已有数据
    for (int i = 0; i < OneDirectionWindRelatedMultipleTree.size(); i++)
    {
        CWindField OnedirectionWind(Common::ProductFrameNumber, OneDirectionWindRelatedMultipleTree[i], 60000, Common::SForceDirection(0, 0), m_MultipleTypeTree[vTreeTypeIndex].getTreeRotationAngle()[i]);
        m_MultipleExtraForces[vTreeTypeIndex].push_back(OnedirectionWind.getDirectionWindForces());
        m_MultipleExtraDirections[vTreeTypeIndex].push_back(OnedirectionWind.getDirectionWindDirection());
    }
}

void CInitMultipleTypeTree::InitASpecificWindSourceWindData(const std::string& vWindSourceConfigPath)
{
    CLoadWindSourceConfig windSource(vWindSourceConfigPath);
    glm::vec3 windCenter = windSource.getWindSourceCenter();
    double windInfluence = windSource.getWindInfluenceArea();
    std::vector<SWaveFunctionPara> SpecificeWindFunctionPara= windSource.getWindSourceFunctionPara();
    std::vector<glm::vec3> WindCenterMoveVelocity=windSource.getWindCenterMoveVelocity();
    std::vector<int> WindCenterMoveFrames= windSource.getWindCenterMoveFrames();
    std::vector<float> MoveScale= windSource.getWindCenterMoveScale();
    m_ASpecificWindSource=new CWindField (windCenter, Common::ProductFrameNumber, SpecificeWindFunctionPara, 60000, windInfluence, WindCenterMoveVelocity, WindCenterMoveFrames, MoveScale);
}

void CInitMultipleTypeTree::SetASpecificWindSourceTreeData(int vTreeTypeIndex)
{

    std::vector<int> TreesNumberSubjected2SameWind = m_MultipleTypeTree[vTreeTypeIndex].getMultipleTreeDuplicateNumber();
    m_MultipleTreesNumberSubjected2SameWind.push_back(TreesNumberSubjected2SameWind);
    std::vector<Common::SForceDirection> tempRotation = m_MultipleTypeTree[vTreeTypeIndex].getTreeRotationAngle();
    m_ASpecificWindSource->setTreePositionAndRotationAngle(m_EachTypeTreesPositionInSence[vTreeTypeIndex], tempRotation);
    m_ASpecificWindSource->setEachTypeTreeNumber(m_MultipleEachTreeProductNumber[vTreeTypeIndex]);
}

void CInitMultipleTypeTree::caculateSpecificWindSourceData()
{
    m_ASpecificWindSource->caculateWindForcesAndDirection2Trees();
    for (int i = 0; i < m_TreeTypeIndex.size(); i++)
    {
        std::vector<std::vector<int>> tempTreeInSameTypeForce;
        std::vector<std::vector<Common::SForceDirection>> tempTreeInSameTypeDirection;
        m_ASpecificWindSource->getSpecificWindForcesAndDirection(i, tempTreeInSameTypeForce, tempTreeInSameTypeDirection);
        m_MultipleExtraForces[i]=tempTreeInSameTypeForce;
        m_MultipleExtraDirections[i] = tempTreeInSameTypeDirection;
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
    std::cout <<"vertexNums"<< vertexNums << std::endl;
    m_MultipleSceneDepthShader[vTreeTypeIndex]->use();
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("frameNums", frameNums);
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("vertexNums", vertexNums);
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("assimpvertexNums", m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber());
    std::cout << "SceneDepth AssimpVerticesvertexNums" << m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber() << std::endl;

    m_MultipleSceneShadowShader[vTreeTypeIndex]->use();
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("frameNums", frameNums);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("vertexNums", vertexNums);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("assimpvertexNums", m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber());
    std::cout << "Scene AssimpVerticesvertexNums" << m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber() << std::endl;

    //create depth cubemap transformation matrices and some value
    m_MultipleSceneDepthShader[vTreeTypeIndex]->use();
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setFloat("far_plane", m_vFarPlane);
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setVec3("lightPos", m_LightVertice);
    for (unsigned int i = 0; i < 6; ++i)
    {
        m_MultipleSceneDepthShader[vTreeTypeIndex]->setMat4("shadowMatrices[" + std::to_string(i) + "]", m_ShadowTransforms[i]);
    }

    //set light to fragment
    m_MultipleSceneShadowShader[vTreeTypeIndex]->use();
    for (unsigned int i = 0; i < sizeof(m_lightVertices) / sizeof(m_lightVertices[0]); ++i)
    {
        glm::vec3 newPos = m_lightVertices[i];
        m_MultipleSceneShadowShader[vTreeTypeIndex]->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        m_MultipleSceneShadowShader[vTreeTypeIndex]->setVec3("lightColors[" + std::to_string(i) + "]", m_lightColors[i]);
    }
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("metallic", 0.04);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("roughness", 0.8);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("ao", 1.0f);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("depthMap", 8);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("far_plane", m_vFarPlane);
}

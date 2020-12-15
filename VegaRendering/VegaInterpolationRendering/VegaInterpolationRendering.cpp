#include <iostream>
#include <GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include "../RenderingProcess/Camera.h"
#include "Shader.h"
#include "Sence.h"
#include "Mesh.h"
#include "VegaFemFactory.h"
#include "../Common/SynchronisedQueue.h"
#include "../Common/WindFiled.h"
#include "LoadWindAndTreeConfig.h"
#include "InitMultipleTypeTree.h"
#include "../Common/ExtraTool.h"
//#include "TreeInstanceMesh.h"
//#include "volumetricMeshLoader.h"
//#include "tetMesh.h"
//#include <vector>
//#include <string>
//#include <cstdio>
//#include <cassert>
//#include <float.h>
//#include "sceneObjectDeformable.h"

void renderPlane(CShader& vShader, const unsigned int& VAOId, const unsigned int& vTextureId, const unsigned int& vTextureOpacityId);
void renderTree(CShader& vShader, CSence& vModel);
void renderLight(CShader& vShader, const unsigned int& VAOId);
void renderSkybox(CShader& vShader, const unsigned int& VAOId, const unsigned int& vTextureId);
bool initWindow(GLFWwindow*& vWindow, int vScreenWidth, int vScreenHeight);
void scrollCallback(GLFWwindow* vWindow, double vXOffset, double vYOffset);
void mouseCallback(GLFWwindow* vWindow, double vXPos, double vYPos);
void framebufferSizeCallback(GLFWwindow* vWindow, int vWidth, int vHeight);
void processInput(GLFWwindow* vWindow);
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(std::vector<std::string> faces);

const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 1000;
bool shadows = true;
bool shadowsKeyPressed = false;

// camera
CCamera Camera(glm::vec3(0, 0.6, 1));
float LastX = SCR_WIDTH / 2.0f;
float LastY = SCR_HEIGHT / 2.0f;
bool FirstMouse = true;

// timing
float DeltaTime = 0.0f;
float LastFrame = 0.0f;

//wind处理优化
int windactive = 0;
float grasstime = 0.0f;

SynchronisedQueue<std::vector<std::pair<int, int>>> SearchQueue[Common::TreesTypeNumber];

std::vector<std::vector<int>> EachFormNumberArray;

int i = 0;
int Size = 0;
int FrameNumber = 0;
////each time change
//int SearchFrameNumber[Common::TreesTypeNumber] = { 0,0,0 };
//int SearchFrameStep[Common::TreesTypeNumber] = { 0,0,0};
int SearchFrameNumber[Common::TreesTypeNumber] = { 0 };
int SearchFrameStep[Common::TreesTypeNumber] = { 0 };



//前一个std::vector表示匹配树的个数，后一个std::vector表示每一帧中需要的数据
//vMultipleExtraForces 表示每一帧风的方向，每次用5帧来进行搜索
//vWindDirection 表示每帧一个风的方向
void InsertSearchTreeFrameIndex(CVegaFemFactory &vVFF, CSence vSence, std::vector<std::vector<int>>& vMultipleExtraForces, std::vector<std::vector<Common::SForceDirection>> & vWindDirection, std::vector<int>& vTreesNumberSubjected2SameWind, int vTreeTypeIndex)
{
    while (true)
    {
        //当前12个帧段进行一次重置获取5个帧段号索引
        /*if (SearchFrameNumber %Size==0)
        {
            std::cout << "search reset" << std::endl;
            vVFF.resetTempMultipleTreeData(vMultipleExtraForces.size());
            SearchFrameStep = 0;
        }*/
        if (SearchFrameNumber[vTreeTypeIndex] == Common::ProductFrameNumber)
        {
            break;
        }

        if (SearchFrameNumber[vTreeTypeIndex] % 5 == 0)
        {
            //每5个力计算一次匹配的5帧
            std::vector<std::vector<int>> tempMultipleFiveForces(vMultipleExtraForces.size());
            std::vector<std::vector<Common::SForceDirection>> tempMultipleFiveWindDirection(vMultipleExtraForces.size());
            for (int i = 0; i < vMultipleExtraForces.size(); i++)
            {
                for (int k = (SearchFrameStep[vTreeTypeIndex]) * 5; k < (SearchFrameStep[vTreeTypeIndex] + 1) * 5; k++)
                {
                    //int t=vMultipleExtraForces[i][k];
                    tempMultipleFiveForces[i].push_back(vMultipleExtraForces[i][k]);
                    tempMultipleFiveWindDirection[i].push_back(vWindDirection[i][k]);
                }
            }
            vVFF.searchMatchedFrameSequences(tempMultipleFiveForces, tempMultipleFiveWindDirection);
            tempMultipleFiveForces.clear();
            tempMultipleFiveWindDirection.clear();
            SearchFrameStep[vTreeTypeIndex]++;
            //std::vector<std::vector<int>> temp = vFem.getMultipleFramesIndex();
        }

        //在这里将重复的搜索数组复制多变
        std::vector<std::pair<int, int>> tempTreeFileAndFrameIndex;

        //std::cout << "TreeIndex" << vTreeTypeIndex << ": ";
        for (int DifferentTreeNumber = 0; DifferentTreeNumber < vTreesNumberSubjected2SameWind.size(); DifferentTreeNumber++)
        {
            std::pair<int, int> tempOneTreeFileAndFrameIndex = vVFF.getFileAndFrameIndex(DifferentTreeNumber, SearchFrameNumber[vTreeTypeIndex] % 5);
            //std::cout << tempOneTreeFileAndFrameIndex.first << "--" << tempOneTreeFileAndFrameIndex.second << "||";

            for (int k = 0; k < vTreesNumberSubjected2SameWind[DifferentTreeNumber]; k++)
            {
                tempTreeFileAndFrameIndex.push_back(tempOneTreeFileAndFrameIndex);
            }
        }
        // std::cout << std::endl;
        SearchQueue[vTreeTypeIndex].Enqueue(tempTreeFileAndFrameIndex);
        SearchFrameNumber[vTreeTypeIndex]++;
        tempTreeFileAndFrameIndex.clear();
    }
}

int main()
{

#pragma region initialize and configure glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // ------------------------------
    GLFWwindow* Window = nullptr;
    if (!initWindow(Window, SCR_WIDTH, SCR_HEIGHT))
    {
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(Window);
    glfwSetFramebufferSizeCallback(Window, framebufferSizeCallback);
    glfwSetCursorPosCallback(Window, mouseCallback);
    glfwSetScrollCallback(Window, scrollCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
#pragma endregion
    /////////////////////////////
#pragma region build and compile shaders
    CShader ourSkyBoxShader("skybox.vert", "skybox.frag");


#pragma endregion

#pragma region plane vertices data
    float planeVertices[] = {
        // positions              // normals       // texture Coords  
         15.0f, -0.5f,  15.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,
        -15.0f, -0.5f,  15.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
        -15.0f, -0.5f, -15.0f,   0.0f, 1.0f, 0.0f,   0.0f, 2.0f,

         15.0f, -0.5f,  15.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,
        -15.0f, -0.5f, -15.0f,   0.0f, 1.0f, 0.0f,   0.0f, 2.0f,
         15.0f, -0.5f, -20.0f,   0.0f, 1.0f, 0.0f,   2.0f, 2.0f,
    };
#pragma endregion

#pragma region skybox vertices data
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
#pragma endregion

#pragma region lights data
    glm::vec3 lightVertices[] = {
        glm::vec3(-3.5f,  4.5f, 1.0f),
        glm::vec3(3.5f,  4.5f, 1.0f),
        glm::vec3(-3.5f,  4.0f, 1.0f),
        glm::vec3(3.5f,  4.0f, 1.0f),
    };

    glm::vec3 lightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };

    float lightPositionsData[] = {
        -1.5f,  2.5f, 1.0f,  1.0f, 0.0f, 0.0f,
        1.5f,  2.5f, 1.0f,	1.0f, 1.0f, 1.0f,
        -1.5f,  2.0f, 1.0f,	1.0f, 1.0f, 1.0f,
        1.5f,  2.5f, 1.0f,	1.0f, 1.0f, 1.0f,
        1.5f,  2.0f, 1.0f,	1.0f, 1.0f, 1.0f,
        -1.5f,  2.0f, 1.0f,	1.0f, 1.0f, 1.0f
    };
#pragma endregion

#pragma region bind light VAO and VBO
    unsigned int lightVAO, lightPotionVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightPotionVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightPotionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightPositionsData), &lightPositionsData, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
#pragma endregion

#pragma region bind plane VAO and VBO
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    // plane load textures
    unsigned int floorTexture = loadTexture("resources/textures/metal.png");
    unsigned int opacityTexture = loadTexture("resources/textures/opacity.png");
#pragma endregion

#pragma region skybox VAO and VBO and Texture
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces
    {
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    };
    // skybox load textures
    unsigned int cubemapTexture = loadCubemap(faces);
    ourSkyBoxShader.use();
    ourSkyBoxShader.setInt("skybox", 0);
#pragma endregion

#pragma region configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion


#pragma region create depth cubemap transformation matrices and some value
    float near_plane = 1.0f;
    float far_plane = 80.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
    std::vector < glm::mat4> shadowTransforms;
    for (unsigned int i = 0; i < 1; ++i)
    {
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightVertices[i], lightVertices[i] + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightVertices[i], lightVertices[i] + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightVertices[i], lightVertices[i] + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightVertices[i], lightVertices[i] + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightVertices[i], lightVertices[i] + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightVertices[i], lightVertices[i] + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    }
    //生成树木的随机位置

//      CInitMultipleTypeTree MultipleTypeTree(Common::TreesTypeNumber, Common::AllTreesNumber);
      ////////////////////////////////////////////
//      MultipleTypeTree.InitShadowCubeMapPara(near_plane, far_plane, SHADOW_WIDTH, SHADOW_HEIGHT, shadowTransforms, lightVertices, lightColors);
      //MultipleTypeTree.InitVegaFemFactory("../../models/yellow_tree/deltaU", "../../models/yellow_tree/tree_last.obj", "../../models/yellow_tree/ObjectVertexIndex.txt", 3);
//      MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[0], "../../models/yellow_tree/WindAndTreeConfig/Config.txt");
//      MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
//      MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.gs");
//      MultipleTypeTree.InitTreeModel("../../models/yellow_tree/tree_last.obj", 0);

    /*  MultipleTypeTree.InitVegaFemFactory("G:/GraduationProject/mini_mapleTree/deltaU", "../../models/mini_mapleTree/tree.obj", "../../models/mini_mapleTree/ObjectVertexIndex.txt", 1);
      MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[0], "G:/GraduationProject/mini_mapleTree/WindAndTreeConfig/Config.txt");
      MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
      MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.gs");
      MultipleTypeTree.InitTreeModel("../../models/mini_mapleTree/tree.obj", 0);*/

      /*  MultipleTypeTree.InitVegaFemFactory("../../models/mini_mapleTree/deltaU", "../../models/mini_mapleTree/tree.obj", "../../models/mini_mapleTree/ObjectVertexIndex.txt",1);
        MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[1], "../../models/mini_mapleTree/WindAndTreeConfig/Config.txt");
        MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
        MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.gs");
        MultipleTypeTree.InitTreeModel("../../models/mini_mapleTree/tree.obj", 1);

        MultipleTypeTree.InitVegaFemFactory("../../models/apricot_tree/deltaU", "../../models/apricot_tree/tree.obj", "../../models/apricot_tree/ObjectVertexIndex.txt", 1);
        MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[2], "../../models/apricot_tree/WindAndTreeConfig/Config.txt");
        MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
        MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.gs");
        MultipleTypeTree.InitTreeModel("../../models/apricot_tree/tree.obj", 2); */

        /* for (int i = 0; i < Common::TreesTypeNumber; i++)
         {
             MultipleTypeTree.InitMultipleExtraWindData(i);
             MultipleTypeTree.InitFemFrameStruct(i);
             MultipleTypeTree.InitScenceShaderData(i);
         }


         for (int i = 0; i < Common::TreesTypeNumber; i++)
         {
             EachFormNumberArray.push_back(MultipleTypeTree.getSpecificLoadWindAndTree(i).getEachFormNumberArray());
         }*/
#pragma endregion
         ////////////////////////////////
    CInitMultipleTypeTree MultipleTypeTree(Common::TreesTypeNumber, Common::AllTreesNumber, false);
    //////////////////////////////////////////
    MultipleTypeTree.InitShadowCubeMapPara(near_plane, far_plane, SHADOW_WIDTH, SHADOW_HEIGHT, shadowTransforms, lightVertices, lightColors);
    MultipleTypeTree.InitVegaFemFactory("../../models/yellow_tree/deltaU", "../../models/yellow_tree/tree_last.obj", "../../models/yellow_tree/ObjectVertexIndex.txt", 1);
    MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[0], "../../models/yellow_tree/HemiWindField/WindSourceTreeConfig.txt");
    MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
    MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.gs");
    MultipleTypeTree.InitTreeModel("../../models/yellow_tree/tree_last.obj", 0, false);
    MultipleTypeTree.InitASpecificWindSourceWindData("../../models/yellow_tree/HemiWindField/WindSourceConfig.txt");
    MultipleTypeTree.InitWindSource("WindSourceCenter.vert", "WindSourceCenter.frag", "../../models/sphere/sphere.obj");

    for (int i = 0; i < Common::TreesTypeNumber; i++)
    {
        MultipleTypeTree.SetASpecificWindSourceTreeData(i);
    }
    MultipleTypeTree.caculateSpecificWindSourceData();

    for (int i = 0; i < Common::TreesTypeNumber; i++)
    {
        MultipleTypeTree.InitFemFrameStruct(i);
        MultipleTypeTree.InitScenceShaderData(i);
    }
    for (int i = 0; i < Common::TreesTypeNumber; i++)
    {
        EachFormNumberArray.push_back(MultipleTypeTree.getSpecificLoadWindAndTree(i).getEachFormNumberArray());
    }


    //开启线程进行读取Tree索引
        /////each time change
    /*boost::thread startInsertIntoQueue = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getFemFactory()), *(MultipleTypeTree.getTreeModel()), *(MultipleTypeTree.getExtraForces()), *(MultipleTypeTree.getExtraDirection()), *(MultipleTypeTree.getTreesNumberSubjected2SameWind()),MultipleTypeTree.getTreeTypeIndex());*/
    boost::thread startInsertIntoQueue = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(0)), *(MultipleTypeTree.getSpecificTreeModel(0)), *(MultipleTypeTree.getSpecificExtraForces(0)), *(MultipleTypeTree.getSpecificExtraDirection(0)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(0)), 0);
    /*boost::thread SecondstartInsertIntoQueue = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(1)), *(MultipleTypeTree.getSpecificTreeModel(1)), *(MultipleTypeTree.getSpecificExtraForces(1)), *(MultipleTypeTree.getSpecificExtraDirection(1)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(1)), 1);
    boost::thread ThirdstartInsertIntoQueue = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(2)), *(MultipleTypeTree.getSpecificTreeModel(2)), *(MultipleTypeTree.getSpecificExtraForces(2)), *(MultipleTypeTree.getSpecificExtraDirection(2)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(2)), 2);*/

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection;
    glm::mat4 view;
    while (!glfwWindowShouldClose(Window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        DeltaTime = (currentFrame - LastFrame);
        LastFrame = currentFrame;

        // input---
        processInput(Window);

        // render----
        glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //每给定的总力段进行一次渲染时的帧D_sum reset
        /*if (FrameNumber%Common::ProductFrameNumber == 0)*/
        /*if (FrameNumber%180 == 0)
        {
            ourModel.resetSSBO4UDeformation();

            std::cout << "//////////////////////////////////////" << std::endl;
            std::cout << "Reset" << std::endl;
        }*/
        std::vector<std::pair<int, int>> tempTreeFileAndFrameIndex;

        for (int i = 0; i < Common::TreesTypeNumber; i++)
        {
            bool Success = SearchQueue[i].TryDequeue(tempTreeFileAndFrameIndex);

            std::cout << "[";
            for (int k = 0; k < EachFormNumberArray[i].size(); k++)
            {
                std::cout << tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].first << "--" << tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].second << "||";
            }
            std::cout << "]";
            MultipleTypeTree.getSpecificTreeModel(i)->UpdataSSBOMeshTreeAndFrameIndex(tempTreeFileAndFrameIndex);
            tempTreeFileAndFrameIndex.clear();
        }
        std::cout << std::endl;
        FrameNumber++;

        // 1. render scene to depth cubemap
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        //plane

        MultipleTypeTree.getSpecificScenceDepthShader(0)->use();
        MultipleTypeTree.getSpecificScenceDepthShader(0)->setInt("planeOrTree", -1);

        renderPlane(*(MultipleTypeTree.getSpecificScenceDepthShader(0)), planeVAO, floorTexture, opacityTexture);
        //tree
        /*MultipleTypeTree.getScenceDepthShader(1)->setInt("planeOrTree", 1);
        renderTree(*(MultipleTypeTree.getScenceDepthShader(1)), *(MultipleTypeTree.getTreeModel(1)));*/
        for (int i = 0; i < Common::TreesTypeNumber; i++)
        {
            MultipleTypeTree.getSpecificTreeModel(i)->UpdataSSBOBindingPointIndex();
            MultipleTypeTree.getSpecificScenceDepthShader(i)->use();
            MultipleTypeTree.getSpecificScenceDepthShader(i)->setInt("planeOrTree", 1);
            renderTree(*(MultipleTypeTree.getSpecificScenceDepthShader(i)), *(MultipleTypeTree.getSpecificTreeModel(i)));
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        //2.render scene as normal 
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ////plane  
        MultipleTypeTree.getSpecificScenceShadowShader(0)->use();
        MultipleTypeTree.getSpecificScenceShadowShader(0)->setInt("shadows", shadows);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        MultipleTypeTree.getSpecificScenceShadowShader(0)->setInt("planeOrTree", -1);
        renderPlane(*(MultipleTypeTree.getSpecificScenceShadowShader(0)), planeVAO, floorTexture, opacityTexture);
        //tree
       /* MultipleTypeTree.getScenceShadowShader(1)->use();
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        MultipleTypeTree.getScenceShadowShader(1)->setInt("planeOrTree", 1);
        renderTree(*(MultipleTypeTree.getScenceShadowShader(1)), *(MultipleTypeTree.getTreeModel(1)));*/
        for (int i = 0; i < Common::TreesTypeNumber; i++)
        {
            MultipleTypeTree.getSpecificTreeModel(i)->UpdataSSBOBindingPointIndex();
            MultipleTypeTree.getSpecificScenceShadowShader(i)->use();
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
            MultipleTypeTree.getSpecificScenceShadowShader(i)->setInt("planeOrTree", 1);
            MultipleTypeTree.getSpecificScenceShadowShader(i)->setFloat("time", glfwGetTime());
            //MultipleTypeTree.getSpecificScenceShadowShader(i)->setInt("frameIndex", frameIndex);
            MultipleTypeTree.getSpecificScenceShadowShader(i)->setInt("sumFaceVerticesBeforeEndMesh", MultipleTypeTree.getSumFaceVerticesBeforeEndMesh(i));
            MultipleTypeTree.getSpecificScenceShadowShader(i)->setInt("waveMap", 9);
            renderTree(*(MultipleTypeTree.getSpecificScenceShadowShader(i)), *(MultipleTypeTree.getSpecificTreeModel(i)));
        }

        if (Common::ShowWindSourceCenter == true)
        {
            projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view = Camera.getViewMatrix();
            MultipleTypeTree.renderingWindSource(projection, view, FrameNumber);
        }
        //skybox	
        ourSkyBoxShader.use();
        renderSkybox(ourSkyBoxShader, skyboxVAO, cubemapTexture);

        Sleep(100);

        glDepthFunc(GL_LESS); // set depth function back to default
        glfwSwapBuffers(Window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &skyboxVBO);
    //glDeleteBuffers(1, &SSBO);
    MultipleTypeTree.getSpecificTreeModel(0)->Clear();
    glfwTerminate();
    return 0;
}

//*********************************************************************
//FUNCTION:
void renderPlane(CShader& vShader, const unsigned int& VAOId, const unsigned int& vTextureId, const unsigned int& vTextureOpacityId)
{
    glm::mat4 projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = Camera.getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    vShader.setMat4("projection", projection);
    vShader.setMat4("view", view);
    vShader.setInt("texture_diffuse1", 0);
    vShader.setInt("texture_opacity1", 1);
    glBindVertexArray(VAOId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vTextureOpacityId);
    vShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//*********************************************************************
//FUNCTION:
void renderTree(CShader & vShader, CSence& vModel)
{
    glm::mat4 projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = Camera.getViewMatrix();
    vShader.setMat4("projection", projection);
    vShader.setMat4("view", view);
    vShader.setVec3("camPos", Camera.getPosition());
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, -0.3f, -2.0f));// translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
    vShader.setMat4("model", model);
    vModel.draw(vShader);

}

//*********************************************************************
//FUNCTION:
void renderLight(CShader & vShader, const unsigned int & VAOId)
{
    glm::mat4 projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = Camera.getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    vShader.setMat4("projection", projection);
    vShader.setMat4("view", view);
    vShader.setMat4("model", model);
    glBindVertexArray(VAOId);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//*********************************************************************
//FUNCTION:
void renderSkybox(CShader & vShader, const unsigned int & VAOId, const unsigned int & vTextureId)
{
    glDepthFunc(GL_LEQUAL);
    //view = Camera.getViewMatrix();//stop move skybox
    glm::mat4  view = glm::mat4(glm::mat3(Camera.getViewMatrix()));
    glm::mat4 projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    vShader.setMat4("view", view);
    vShader.setMat4("projection", projection);
    glBindVertexArray(VAOId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, vTextureId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

//*********************************************************************
//FUNCTION:
bool initWindow(GLFWwindow*& vWindow, int vScreenWidth, int vScreenHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    vWindow = glfwCreateWindow(vScreenWidth, vScreenHeight, "LearnOpenGL", NULL, NULL);
    if (vWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(vWindow);
    glfwSetFramebufferSizeCallback(vWindow, framebufferSizeCallback);
    glfwSetCursorPosCallback(vWindow, mouseCallback);
    glfwSetScrollCallback(vWindow, scrollCallback);
    glfwSetInputMode(vWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "glew init failed." << std::endl;
        glfwTerminate();
        return false;
    }
    return true;
}

//*********************************************************************
//FUNCTION:
void processInput(GLFWwindow* vWindow)
{
    if (glfwGetKey(vWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(vWindow, true);

    float cameraSpeed = 2.5f * DeltaTime; // adjust accordingly
    if (glfwGetKey(vWindow, GLFW_KEY_W) == GLFW_PRESS)
        Camera.processKeyboard(FORWARD, DeltaTime);
    if (glfwGetKey(vWindow, GLFW_KEY_S) == GLFW_PRESS)
        Camera.processKeyboard(BACKWARD, DeltaTime);
    if (glfwGetKey(vWindow, GLFW_KEY_A) == GLFW_PRESS)
        Camera.processKeyboard(LEFT, DeltaTime);
    if (glfwGetKey(vWindow, GLFW_KEY_D) == GLFW_PRESS)
        Camera.processKeyboard(RIGHT, DeltaTime);

    if (glfwGetKey(vWindow, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
    {
        shadows = !shadows;
        shadowsKeyPressed = true;
    }
    if (glfwGetKey(vWindow, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        shadowsKeyPressed = false;
    }
}

//*********************************************************************
//FUNCTION:
void mouseCallback(GLFWwindow* vWindow, double vXPos, double vYPos)
{
    if (FirstMouse)
    {
        LastX = vXPos;
        LastY = vYPos;
        FirstMouse = false;
    }

    float XOffset = vXPos - LastX;
    float YOffset = LastY - vYPos;
    LastX = vXPos;
    LastY = vYPos;

    Camera.processMouseMovement(XOffset, YOffset);
}

//*********************************************************************
//FUNCTION:
void scrollCallback(GLFWwindow* vWindow, double vXOffset, double vYOffset)
{
    Camera.processMouseScroll(vYOffset);
}

//*********************************************************************
//FUNCTION:
void framebufferSizeCallback(GLFWwindow* vWindow, int vWidth, int vHeight)
{
    glViewport(0, 0, vWidth, vHeight);
}
// utility function for loading a 2D texture from file
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


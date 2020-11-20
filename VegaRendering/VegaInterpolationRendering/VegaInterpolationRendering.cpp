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

SynchronisedQueue<std::vector<std::pair<int, int>>> SearchQueue;

int i = 0;
int Size = 0;
int FrameNumber = 0;
int SearchFrameNumber = 0;
int SearchFrameStep = 0;

void InsertSearchTreeFrameIndex(CVegaFemFactory &vVFF, CSence vSence, std::vector<std::vector<int>>& vMultipleExtraForces)
{
	while (true)
	{
		//当前12个帧段进行一次重置获取5个帧段号索引
	/*	if (SearchFrameNumber %Size==0)
		{
			std::cout << "search reset" << std::endl;
			vVFF.resetTempMultipleTreeData(vMultipleExtraForces.size());
			SearchFrameStep = 0;
		}*/
		if (SearchFrameNumber % 5 == 0)
		{
			//每5个力计算一次匹配的5帧
			std::vector<std::vector<int>> tempMultipleFiveForces(vMultipleExtraForces.size());
			for (int i = 0; i < vMultipleExtraForces.size(); i++)
			{
				for (int k = (SearchFrameStep) * 5; k < (SearchFrameStep + 1) * 5; k++)
				{
					tempMultipleFiveForces[i].push_back(vMultipleExtraForces[i][k]);
				}
			}
			vVFF.searchMatchedFrameSequences(tempMultipleFiveForces);
			tempMultipleFiveForces.clear();
			SearchFrameStep++;
			//std::vector<std::vector<int>> temp = vFem.getMultipleFramesIndex();
		}
		std::vector<std::pair<int, int>> tempTreeFileAndFrameIndex;
		for (int treenumber = 0; treenumber < Common::TreesNumber; treenumber++)
		{
			tempTreeFileAndFrameIndex.push_back(vVFF.getFileAndFrameIndex(treenumber, SearchFrameNumber % 5));

			//std::cout << tempTreeFileAndFrameIndex[treenumber].first << "--" << tempTreeFileAndFrameIndex[treenumber].second << "||";
		}
		//std::cout << std::endl;
		SearchQueue.Enqueue(tempTreeFileAndFrameIndex);
		SearchFrameNumber++;
		tempTreeFileAndFrameIndex.clear();
	}
}

int main()
{
	CVegaFemFactory vFem("G:/GraduationProject/yellow_tree/deltaU", "../../models/yellow_tree/tree_last.obj", "../../models/yellow_tree/ObjectVertexIndex.txt");
	int numbercounter = 3;
	std::vector<double> b{ 1000,1,0,0 };
	std::vector<std::pair<int, int>> angle;
	for (int i = 0; i < numbercounter; i++)
	{
		angle.push_back(std::make_pair(0, i * 60));
	}
	//std::vector<Common::SFileFrames> vtemp = vFem.searchFileFramesOnAnimation(angle[i].first, angle[i].second, b);
	//std::vector<Common::SFileFrames> vtemp = vFem.searchFileFrameOnAttribute();
	for (int i = 0; i < numbercounter; i++)
	{
		std::vector<Common::SFileFrames> vtemp = vFem.searchFileFramesOnAnimation(angle[i].first, angle[i].second, b);
		/*std::vector<Common::SFileFrames> temp;
		temp.push_back(vtemp[i]);*/
		vFem.readFramesDeformationData(vtemp, i);
	}

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

#pragma region build and compile shaders
	CShader ourSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
	CShader ourSkyBoxShader("skybox.vert", "skybox.frag");
	CShader ourSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.gs");
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
		glm::vec3( 3.5f,  4.5f, 1.0f), 
		glm::vec3(-3.5f,  4.0f, 1.0f),
		glm::vec3( 3.5f,  4.0f, 1.0f),
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

#pragma region load model
	CSence ourModel("../../models/yellow_tree/tree_last.obj");
	ourModel.setMeshRotation();
	ourModel.setGroupsIndex(vFem);
	ourModel.setVerticesNumber(vFem);
	ourModel.setMeshGroupAndAssimpIndex();
	ourModel.initSSBODeformationDeltaU(vFem, numbercounter);
	ourModel.initSSBODeformationU();
	ourModel.initSSBOTreeFileAndFrameIndex(Common::TreesNumber);
	ourModel.setSSBO4UDeformationAndIndex(ourSceneShadowShader);
	ourModel.setSSBOUdeformationAndIndx4ShadowMapShader(ourSceneDepthShader);
#pragma endregion



	//个数等于
	std::vector<std::vector<int>> vMultipleExtraForces;
	for (int i = 0; i < 2; i++)
	{
		vMultipleExtraForces.push_back(GenerateSamplingForce(Common::ProductFrameNumber,1000, 1, 0, 0, 6000));
	}
	/*for (int i = 0; i < 15; i++)
	{
		vMultipleExtraForces.push_back(GenerateSamplingForce(Common::ProductFrameNumber, 1010, 1, 0.5, 0, 600));
	}
	for (int i = 0; i < 20; i++)
	{
		vMultipleExtraForces.push_back(GenerateSamplingForce(Common::ProductFrameNumber, 600, 1, 0.25, 0, 600));
	}*/
	/*for (int i = 0; i < 15; i++)
	{
		vMultipleExtraForces.push_back(GenerateSamplingForce(Common::ProductFrameNumber, 2500, 1, 0.25, 0, 600));
	}*/

	//Size = Common::ProductFrameNumber;
	//Size = 180;
	vFem.initMatchedFrameStruct(vMultipleExtraForces.size());
	vFem.initKVFDataSearchRangeError();

	//帧数
	int frameNums = vFem.getFileFrames(0)->Frames.size();
	//obj model vertices
	int vertexNums = vFem.getFileFrames(0)->Frames[0].BaseFileDeformations.size();
	ourSceneDepthShader.use();
	ourSceneDepthShader.setInt("frameNums", frameNums);
	ourSceneDepthShader.setInt("vertexNums", vertexNums);
	ourSceneDepthShader.setInt("assimpvertexNums", ourModel.getAssimpVerticesNumber());
	ourSceneShadowShader.use();
	ourSceneShadowShader.setInt("frameNums", frameNums);
	ourSceneShadowShader.setInt("vertexNums", vertexNums);
	ourSceneShadowShader.setInt("assimpvertexNums", ourModel.getAssimpVerticesNumber());
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
		ourSceneDepthShader.use();
		ourSceneDepthShader.setFloat("far_plane", far_plane);
		ourSceneDepthShader.setVec3("lightPos", lightVertices[0]);
		for (unsigned int i = 0; i < 6; ++i)
		{
			ourSceneDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}

	#pragma endregion

#pragma region set light to fragment
	ourSceneShadowShader.use();
	for (unsigned int i = 0; i < sizeof(lightVertices) / sizeof(lightVertices[0]); ++i)
	{
		glm::vec3 newPos = lightVertices[i];
		ourSceneShadowShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		ourSceneShadowShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
	}
	ourSceneShadowShader.setFloat("metallic", 0.04);
	ourSceneShadowShader.setFloat("roughness", 0.8);
	ourSceneShadowShader.setFloat("ao", 1.0f);
	ourSceneShadowShader.setInt("depthMap", 8);
	ourSceneShadowShader.setFloat("far_plane", far_plane);

#pragma endregion


	//开启线程进行读取Tree索引
	boost::thread startInsertIntoQueue = boost::thread(InsertSearchTreeFrameIndex,vFem, ourModel, vMultipleExtraForces);

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
		bool Success = SearchQueue.TryDequeue(tempTreeFileAndFrameIndex);
		ourModel.UpdataSSBOMeshTreeAndFrameIndex(tempTreeFileAndFrameIndex);
		tempTreeFileAndFrameIndex.clear();
		FrameNumber++;

		// 1. render scene to depth cubemap
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//plane
		ourSceneDepthShader.use();
		ourSceneDepthShader.setInt("planeOrTree", -1);
		renderPlane(ourSceneDepthShader, planeVAO, floorTexture, opacityTexture);
		//tree
		ourSceneDepthShader.setInt("planeOrTree", 1);
		renderTree(ourSceneDepthShader, ourModel);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//2.render scene as normal 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//plane
		ourSceneShadowShader.use();
		ourSceneShadowShader.setInt("shadows", shadows);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		ourSceneShadowShader.setInt("planeOrTree", -1);
		renderPlane(ourSceneShadowShader, planeVAO, floorTexture, opacityTexture);
		//tree
		ourSceneShadowShader.use();
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		ourSceneShadowShader.setInt("planeOrTree", 1);
		renderTree(ourSceneShadowShader, ourModel);
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
	ourModel.Clear();
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
	model = glm::translate(model, glm::vec3(0.0f, -0.3f, -2.0f));// translate it down so it's at the center of the scene
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


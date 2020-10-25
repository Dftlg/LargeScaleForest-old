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
//#include "TreeInstanceMesh.h"
//#include "volumetricMeshLoader.h"
//#include "tetMesh.h"
//#include <vector>
//#include <string>
//#include <cstdio>
//#include <cassert>
//#include <float.h>
//#include "sceneObjectDeformable.h"

bool initWindow(GLFWwindow*& vWindow, int vScreenWidth, int vScreenHeight);
void scrollCallback(GLFWwindow* vWindow, double vXOffset, double vYOffset);
void mouseCallback(GLFWwindow* vWindow, double vXPos, double vYPos);
void framebufferSizeCallback(GLFWwindow* vWindow, int vWidth, int vHeight);
void processInput(GLFWwindow* vWindow);
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(std::vector<std::string> faces);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
CCamera Camera(glm::vec3(0, 0, 0));
float LastX = SCR_WIDTH / 2.0f;
float LastY = SCR_HEIGHT / 2.0f;
bool FirstMouse = true;

// timing
float DeltaTime = 0.0f;
float LastFrame = 0.0f;

//wind处理优化
int windactive = 0;
float grasstime = 0.0f;

int main()
{
	CVegaFemFactory vFem("../../models/8.10/test4", "../../models/8.10/1.obj", "../../models/8.10/ObjectVertexIndex.txt");
	std::vector<int> b{ 200, 1, 0 };
	std::vector<std::pair<int, int>> angle;
	int numbercounter = 4;
	bool interpolationOnAnimation = false, interpolationOnAttribute = false;
	for (int i = 0; i < numbercounter; i++)
	{
		angle.push_back(std::make_pair(0, i * 30));
	}

	std::vector<Common::SFileFrames> vtemp = vFem.searchFileFrameOnAttribute();
	for (int i = 0; i < numbercounter; i++)
	{
		//给定角度下相关联的一些位移帧文件集合，但由于目前只有一个，就每个角度特定对应一个
		//std::vector<Common::SFileFrames> vtemp = vFem.searchFileFrames(angle[i].first, angle[i].second, b);
		std::vector<Common::SFileFrames> temp;
		temp.push_back(vtemp[i]);
		vFem.readFramesDeformationData(temp, i);//i本来应该是vtemp.size()
	}

	// glfw: initialize and configure

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	glEnable(GL_DEPTH_TEST);
	// build and compile shaders
	// -------------------------

	CShader ourTreeShader("Tree.vert", "Tree.frag");
	CShader ourPlaneShader("Plane.vert", "Plane.frag");
	CShader ourSkyBoxShader("skybox.vert", "skybox.frag");



	//plane vertices
	float planeVertices[] = {
		// positions          // texture Coords 
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	//skybox vertices
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

	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
	// plane load textures
	unsigned int floorTexture = loadTexture("resources/textures/metal.png");
	ourPlaneShader.use();
	ourPlaneShader.setInt("texture1", 0);

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

	CSence ourModel("../../models/8.10/1.obj");

	ourModel.setMeshRotation();
	ourModel.setGroupsIndex(vFem);
	ourModel.setVerticesNumber(vFem);
	ourModel.setMeshGroupAndAssimpIndex();
	


	//查找帧段
	std::vector<std::vector<glm::vec3>> matchedFramesSequences;
	std::vector<int>vExtraForces = GenerateSamplingForce(Common::ProductFrameNumber,130, 1, 0, 0,4);
	Common::SpKVFData voSpKVData;
	bool flag = true;
	vFem.searchMatchedFrameSegment(matchedFramesSequences,voSpKVData,vExtraForces, flag);
	//帧数
	int frameNums = matchedFramesSequences.size();
	//obj模型的顶点数
	int vertexNums = matchedFramesSequences[0].size();
	std::cout << frameNums << " " << vertexNums << std::endl;
	glm::vec4* deformU = new glm::vec4[frameNums*vertexNums];

	
	
	for (int i = 0; i < frameNums; i++)
	{
		for (int k = 0; k < vertexNums; k++)
		{
			deformU[i * vertexNums + k] = glm::vec4(matchedFramesSequences[i][k] , 0.0f);
		}
	}


	////帧数
	//int frameNums = vFem.getFileFrames(0).Frames.size();
	////obj模型的顶点数
	//int vertexNums = vFem.getFileFrames(0).Frames[0].BaseFileDeformations.size();

	//std::cout << frameNums << " " << vertexNums << std::endl;
	//glm::vec4* deformU = new glm::vec4[frameNums*vertexNums*numbercounter];
 	//int count = 0;
	//for (int j = 0; j < numbercounter; j++)
	//{
	//	for (int i = 0; i < vFem.getFileFrames(j).Frames.size(); i++)
	//	{
	//		Common::SFileData frame = vFem.getFileFrames(j).Frames[i];
	//		for (int k = 0; k < frame.BaseFileDeformations.size(); k++)
	//		{
	//			deformU[j*frameNums*vertexNums + i * vertexNums + k] = glm::vec4(frame.BaseFileDeformations[k], 0.0f);
	//			count++;
	//		}
	//	}
	//}
	//std::cout << deformU[numbercounter*frameNums*vertexNums - 1].x << " " << count << std::endl;

	GLuint shader_index = glGetProgramResourceIndex(ourTreeShader.getID(), GL_SHADER_STORAGE_BLOCK, "DeformationArray");
	GLint SSBOBinding = 0, BlockDataSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize);

	//初始化SSBO
	unsigned int SSBO;
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*frameNums*vertexNums, deformU, GL_STATIC_DRAW);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*frameNums*vertexNums*numbercounter, deformU, GL_STATIC_DRAW);

	//shader和点连接
	GLuint ssbo_binding_point_index = 1;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index, SSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(ourTreeShader.getID(), shader_index, ssbo_binding_point_index);


	glm::vec4* deltaU = new glm::vec4[ourModel.getAssimpVerticesNumber()];
	GLuint shader_delta_index = glGetProgramResourceIndex(ourTreeShader.getID(), GL_SHADER_STORAGE_BLOCK, "DeltaDeformationArray");
	GLint SSBOBinding1 = 0, BlockDataSize1 = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding1);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize1);

	unsigned int deltaSSBO;
	glGenBuffers(1, &deltaSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, deltaSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(ourModel.getAssimpVerticesNumber()), deltaU, GL_DYNAMIC_DRAW);
	GLuint deltassbo_binding_point_index = 2;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, deltassbo_binding_point_index, deltaSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(ourTreeShader.getID(), shader_delta_index, deltassbo_binding_point_index);

	ourTreeShader.use();
	ourTreeShader.setInt("frameNums", frameNums);
	ourTreeShader.setInt("vertexNums", vertexNums);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 projection;
	glm::mat4 view;
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(1.0f, -0.5f, 0.0f));// translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
	//ourShader.setMat4("model", model);

	int i = 0;
	while (!glfwWindowShouldClose(Window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		DeltaTime = (currentFrame - LastFrame);
		LastFrame = currentFrame;

		// input---
		processInput(Window);

		// render----
		glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//plane
		ourPlaneShader.use();
		projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = Camera.getViewMatrix();
		ourPlaneShader.setMat4("projection", projection);
		ourPlaneShader.setMat4("view", view);
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		ourPlaneShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//tree
		ourTreeShader.use();
		projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = Camera.getViewMatrix();
		ourTreeShader.setMat4("projection", projection);
		ourTreeShader.setMat4("view", view);	
		ourTreeShader.setInt("frameIndex", i);

	/*	if (i >= Common::CorrectuDeformationFrame)
		{
			glm::vec4* tempU = new glm::vec4[ourModel.getAssimpVerticesNumber()];
			glGetBufferSubData(deltaSSBO, 0, sizeof(glm::vec4)*ourModel.getAssimpVerticesNumber(), tempU);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, deltaSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4)*ourModel.getAssimpVerticesNumber(), deltaU);
		}*/

		if (i >= frameNums)
		{
			i = i % frameNums;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, deltaSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4)*ourModel.getAssimpVerticesNumber(), deltaU);
		}
		for (int j = 0; j < 1; j++)
		{

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(1.0f*j, -0.5f, -5.0f));// translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			ourTreeShader.setMat4("model", model);
			ourTreeShader.setInt("treeIndex", j);
			ourModel.draw(ourTreeShader);
		}
		//ourModel.draw(ourShader)
		i++;

		//skybox
		glDepthFunc(GL_LEQUAL);
		ourSkyBoxShader.use();
		view = glm::mat4(glm::mat3(Camera.getViewMatrix()));
		ourSkyBoxShader.setMat4("view", view);
		ourSkyBoxShader.setMat4("projection", projection);
		//
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		glfwSwapBuffers(Window);
		glfwPollEvents();

		Sleep(100);

	}

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &SSBO);
	glfwTerminate();
	return 0;
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
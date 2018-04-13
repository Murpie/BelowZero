#pragma once
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"
#include <GL/gl3w.h>   
#include "glfw\include\GLFW\glfw3.h"
#include <vector>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <random>
#include "stb_image.h"
#include "GameScene.h"
#include "Player.h"
#include "ShaderProgramLib.h"
#define STB_IMAGE_IMPLEMENTATION

// Cube Map defines for its positions
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X  0x8515 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X  0x8516 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y  0x8517 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y  0x8518 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z  0x8519 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  0x851A

struct QuadVertex
{
	float x, y, u, v;
};

class RenderManager
{
public:
	RenderManager();
	RenderManager(GameScene *otherGameScene, GLFWwindow* otherWindow, ShaderProgramLib* shaderProgram);
	~RenderManager();

	GameScene *gameScene;
	Player player;
	void FindObjectsToRender();
	void Render();
	void createBuffers();
	void renderQuad();
	void renderSkyQuad();
	void Update();
	void setDeltaTime(float deltaTime);
	void setSeconds(float seconds);
	void setupMatrices(unsigned int shaderToUse, glm::vec3 lightPos);
	void setupMatricesForCubeMapShadowMap(unsigned int shaderToUse, glm::vec3 lightPosition);

private:
	unsigned int loadCubemap(std::vector<std::string> faces);

	std::vector<GameObject*> gameObjectsToRender;
	
	std::vector<Light*> lightsToRender;
	std::vector<glm::vec3> kernel;

	glm::mat4x4 view_matrix;
	glm::mat4x4 projection_matrix;
	glm::mat4x4 currentCubeMapView;

	float deltaTime;
	float seconds;
	int count;

	unsigned int UIFBO;
	unsigned int UITexture;
	unsigned int shadowMap;
	unsigned int shadowFBO;
	unsigned int cubeMapShadowMap;
	unsigned int cubeMapShadowFBO;
	unsigned int cubeVAO;
	unsigned int cubeVBO;
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int skyboxEBO;
	unsigned int animationVAO;
	unsigned int animationVBO;
	unsigned int animationEBO;
	unsigned int ebo;
	unsigned int vbo;
	unsigned int vao;
	unsigned int gbo;
	unsigned int skyFBO;
	unsigned int rboDepth;
	unsigned int finalFBO;
	unsigned int finalColorBuffer;
	unsigned int finalDepthStensil;

	unsigned int equipedFBO;
	unsigned int equipedTexture;

	unsigned int cubemapTexture;
	unsigned int gAlbedo;
	unsigned int gNormal;
	unsigned int gPosition;
	unsigned int gSpecular;
	unsigned int gMetallic;
	unsigned int gAO;
	unsigned int attachments[6] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5 };

	std::vector<std::string> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	QuadVertex vertices;
	unsigned int indices;
	GLFWwindow* window;

	GLuint shadowMapShaderProgram;
	GLuint pointLightShaderProgram;
	GLuint geometryShaderProgram;
	GLuint cubeMapShaderProgram;
	GLuint lightpassShaderProgram;
	GLuint skyboxShaderProgram;
	GLuint animationShaderProgram;
	GLuint UIShaderProgram;

	int display_w, display_h;
	unsigned int cubeMapSize = 64;

	float skyboxArray[72] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,

		-1.0f, -1.0f, -1.0f,//BL
		-1.0f, -1.0f,  1.0f,//BR
		1.0f, -1.0f, -1.0f,//FL
		1.0f, -1.0f,  1.0f//FR
	};

	unsigned int skyboxIndices[36] = {
		// positions          
		0, 1, 2, //Left
		2, 3, 0,

		4, 5, 6, //Back
		6, 7, 4,

		8, 9, 10, //Front
		10, 11, 8,

		12, 13, 14, //Right
		14, 15, 12,

		16, 17, 18, //Top
		18, 19, 16,

		20, 21, 22, //Bottom
		21, 23, 22
	};

	float cubeVertices[216] = {
		// positions          // normals
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,//LEFT
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,

		0.5f, 0.5f,  0.5f,  0.0f,  0.0f, -1.0f,//RIGHT
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, 0.5f,  0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f,  -0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,//FRONT
		-0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,  0.0f,  0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
		-0.5f,  -0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,//BACK
		0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

		0.5f, -0.5f, 0.5f,  0.0f, 1.0f,  0.0f,//BOTTOM
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,
		-0.5f, -0.5f,  -0.5f,  0.0f, 1.0f,  0.0f,
		-0.5f, -0.5f,  -0.5f,  0.0f, 1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
		0.5f, -0.5f, 0.5f,  0.0f, 1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f,//TOP
		0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  -1.0f,  0.0f
	};

};

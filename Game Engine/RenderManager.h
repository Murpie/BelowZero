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
#include "VFX.h"
#define STB_IMAGE_IMPLEMENTATION

// Cube Map defines for its positions
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X  0x8515 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X  0x8516 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y  0x8517 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y  0x8518 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z  0x8519 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  0x851A

#define LOW_SHADOW 1024
#define MEDIUM_SHADOW 2048
#define HIGH_SHADOW 4096

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
	void FindObjectsToRender();
	void Render();
	void createBuffers();
	void renderQuad();
	void Update();
	void setDeltaTime(float deltaTime);
	void setSeconds(float seconds);
	void setupMatrices(unsigned int shaderToUse, glm::vec3 lightPos);
	int FindUnusedParticle();
	/*void setupMatricesForCubeMapShadowMap(unsigned int shaderToUse, glm::vec3 lightPosition);*/

private:
	/*unsigned int loadCubemap(std::vector<std::string> faces);*/

	std::vector<GameObject*> gameObjectsToRender;
	
	std::vector<Light*> lightsToRender;
	std::vector<glm::vec3> kernel;

	glm::mat4x4 view_matrix;
	glm::mat4x4 fpsView_matrix;
	glm::mat4x4 projection_matrix;
	glm::mat4x4 currentCubeMapView;

	float deltaTime;
	float seconds;
	int count;

	unsigned int UIFBO;
	unsigned int UITexture;
	unsigned int shadowMap;
	unsigned int shadowFBO;
	//unsigned int cubeMapShadowMap;
	//unsigned int cubeMapShadowFBO;
	unsigned int animationVAO;
	unsigned int animationVBO;
	unsigned int animationEBO;
	unsigned int ebo;
	unsigned int vbo;
	unsigned int vao;
	unsigned int gbo;
	unsigned int rboDepth;
	unsigned int finalFBO;
	unsigned int finalColorBuffer;
	unsigned int finalDepthStensil;
	unsigned int billboardVAO;
	unsigned int particlePositionBuffer;
	unsigned int particleColorBuffer;

	unsigned int lastUsedParticle;
	unsigned int particleCount = 100;
	unsigned int newParticles = (int)(deltaTime * 100);
	const int maxParticles = 10000;
	VFX::Particle* particleContainer;

	GLvoid* particlePositionData = NULL;
	GLvoid* particleColorData = NULL;

	unsigned int equipedFBO;
	unsigned int equipedTexture;

	//unsigned int cubemapTexture;
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

	QuadVertex vertices;
	GLFWwindow* window;

	GLuint shadowMapShaderProgram;
	GLuint geometryShaderProgram;
	GLuint cubeMapShaderProgram;
	GLuint lightpassShaderProgram;
	GLuint skyboxShaderProgram;
	GLuint animationShaderProgram;
	GLuint UIShaderProgram;

	int display_w, display_h;
};

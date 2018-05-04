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
#include "ShaderProgramLib.h"
#include "TextureLib.h"
#include <math.h>
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

#define MAX_PARTICLES 1000

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
	void clearObjectsToRender();

	void Render();
	void renderMainMenu();
	void createBuffers();
	void createMainMenuBuffer();
	void createButtonQuads();
	void deleteData();
	void renderQuad();
	void Update();
	void setDeltaTime(float deltaTime);
	void setSeconds(float seconds);
	void setupMatrices(unsigned int shaderToUse, glm::vec3 lightPos);
	void renderParticles();

	struct Particle {
		glm::vec3 pos, speed;
		unsigned char r, g, b, a;
		float size, angle, weight;
		float life;
		float cameraDistance;
	};

	Particle particleContainer[MAX_PARTICLES];

	int FindUnusedParticle(Particle* container, int lastUsedParticle);
	void ParticleLinearSort(Particle* arr, int size);

private:

	std::vector<GameObject*> gameObjectsToRender;
	std::vector<Light*> lightsToRender;

	glm::mat4x4 view_matrix;
	glm::mat4x4 fpsView_matrix;
	glm::mat4x4 projection_matrix;
	glm::mat4x4 currentCubeMapView;

	GLuint finalMainMenuFBOTexture;

	float deltaTime;
	float seconds;
	int count;

	unsigned int finalMainMenuFBO;
	unsigned int quadVertexArrayObject;
	unsigned int quadVertexBufferObject;
	unsigned int buttonVertexArrayObject[3];
	unsigned int buttonBufferObject[3];

	int width, height, nrOfChannels;
	unsigned char* data;

	unsigned int UIFBO;
	unsigned int UITexture;
	unsigned int shadowMap;
	unsigned int shadowFBO;
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
	unsigned int billboardTexture;
	unsigned int billboard_vertex_array;
	unsigned int billboard_vertex_buffer;
	unsigned int particlePositionBuffer;
	unsigned int particleColorBuffer;

	unsigned int lastUsedParticle = 0;
	unsigned int particleCount = 10;
	unsigned int newParticles = 1;
	unsigned int randomizer = 0;
	float defaultX = 30.0f;
	float defaultY = 8.0f;
	float defaultZ = 50.0f;
	float offset = 15.0f;
	int distanceToParticles = 0;
	glm::vec3 startPoint;
	glm::vec3 particlePivot;
	GLfloat* particlePositionData = 0;
	GLubyte* particleColorData = 0;

	unsigned int equipedFBO;
	unsigned int equipedTexture;

	unsigned int gAlbedo;
	unsigned int gPosition;
	unsigned int gNormal;
	unsigned int attachments[3] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
	};

	QuadVertex vertices;
	GLFWwindow* window;

	GLuint shadowMapShaderProgram;
	GLuint geometryShaderProgram;
	GLuint lightpassShaderProgram;
	GLuint animationShaderProgram;
	GLuint UIShaderProgram;
	GLuint terrainShaderProgram;
	GLuint vfxShaderProgram;
	GLuint mainMenuShaderProgram;

	int display_w, display_h;
	unsigned int vertexPos;
	unsigned int uvPos;
};
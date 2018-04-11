#pragma once

#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include "glfw\include\GLFW\glfw3.h"
#include <stdio.h>
#include <time.h>

/////////////

#include "TextureLib.h"
#include "ShaderProgramLib.h"
#include "GameScene.h"
#include "RenderManager.h"
#include "Transform.h"
#include "MeshFilter.h"
//#include "CharacterMovement.h"
#include <string>
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include "glm\glm\glm.hpp"
//#include "Light.h"
#include <ctime>
#include "GeometryShaders.h"
#include "SSAOShaders.h"
#include "SSAOBlurShaders.h"
#include "LightpassShaders.h"
#include "FXAAShaders.h"
#include "CubeMapShaders.h"

#include <chrono>
#include "PointLightShadowMapShaders.h"
////////////

class Game
{
public:
	Game();
	~Game();

	void run();

private:
	GLFWwindow * window;
	GameScene gameScene;
	ShaderProgramLib shaderProgramLibrary;
	MaterialLib materialLibrary;
	TextureLib textureLibrary;
	MeshLib meshLibrary;
	
	//
	vector<RenderManager> renderManager; // use 1 render manager and 
	vector<string> meshName; // Filepaths for the importer?

	Gamestate::ID stateOfGame; // EnumID.h

	void printCurrentState(Gamestate::ID stateOfGame);
	void runState();

	char windowName[20];

	void initWindow();
	void initScene(GameScene &scene);
	void initShaderProgramLib();
	void initInputOptions();

	void useShaderProgram();

	void addMeshName();

	//...
	void addLights(GameScene &scene);
	void addRenderManager(GameScene &scene);
	void addCharacterMovement(GameScene &scene);
	void addMeshFilter(GameScene &scene);
	void readMeshName(GameScene &scene);

	//...
	void processInput(GLFWwindow *window, float deltaTime); //GameScene &scene

	//Time Variables
	float deltaTime;
	float seconds;
	//Shader bools
	bool gaussianblur;
	bool fxaa;
	bool ssao;
};


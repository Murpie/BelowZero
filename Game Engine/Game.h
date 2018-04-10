#pragma once

#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include "glfw\include\GLFW\glfw3.h"
#include <stdio.h>
#include <time.h>

/////////////
#include "MaterialLib.h"
#include "TextureLib.h"
#include "ShaderProgramLib.h"
#include "MeshLib.h"
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
	//GameScene gameScene; // maybe  vector<GameScene> and switch between gameScene[i].update() or just clear the gamescene and reuse
	//GameScene menuScene; // temporary GameScene
	vector<GameScene> gameScenes; // test
	ShaderProgramLib shaderProgramLibrary;
	MaterialLib materialLibrary;
	TextureLib textureLibrary;
	MeshLib meshLibrary;
	
	//
	//vector<Light> lights; // rework this solution or figure out how to keep track on lights for multiple scenes. 
	//vector<CharacterMovement> moveScript; // same as for lights, mabe create this local in the initScene() and pass it to the functions
	vector<RenderManager> renderManager;
	vector<string> meshName;
	vector<MeshFilter> meshFilter;

	Gamestate::ID stateOfGame; // EnumID.h
	GameScene& getGameScene(Scene::ID sceneID);
	void deleteGameScene(Scene::ID sceneID);

	void printCurrentState(Gamestate::ID stateOfGame);
	void runState();

	char windowName[20];

	void initWindow();
	void initScene(GameScene &scene);
	void initShaderProgramLib();
	void initInputOptions();

	void useShaderProgram();

	void addMeshName();
	void addGameScene(Scene::ID sceneID);

	//...
	void addLights(GameScene &scene);
	void addRenderManager(GameScene &scene);
	void addCharacterMovement(GameScene &scene);
	void addMeshFilter(GameScene &scene);
	void readMeshName(GameScene &scene);
	void setGameObjects(GameScene &scene);

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


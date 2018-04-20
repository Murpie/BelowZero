#pragma once

// random comment

#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include "glfw\include\GLFW\glfw3.h"
#include <stdio.h>
#include <time.h>
#include <ctime>

/////////////

#include "TextureLib.h"
#include "ShaderProgramLib.h"
#include "GameScene.h"
#include "RenderManager.h"
//#include "Transform.h"
#include "MeshFilter.h"
//#include "CharacterMovement.h"
#include <string>
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include "glm\glm\glm.hpp"
//#include "Light.h"
#include <ctime>
#include "GeometryShaders.h"
#include "LightpassShaders.h"
#include "CubeMapShaders.h"

#include <chrono>
#include "PointLightShadowMapShaders.h"
////////////

class Game
{
public:
	Game();
	~Game();

	void run(); // Main function that holds the main loop

private:
	GLFWwindow * window;
	GLFWmonitor** primary;
	const GLFWvidmode* mode;

	GameScene gameScene;
	GameScene menuScene;

	ShaderProgramLib shaderProgramLibrary;
	MaterialLib materialLibrary;
	TextureLib textureLibrary;
	MeshLib meshLibrary;
	
	//
	vector<RenderManager> renderManager; // use 1 render manager? delete and init new one
	vector<string> meshName; // Filepaths for the importer?

	Gamestate::ID stateOfGame; // EnumID.h, make this one global(?) 

	void printCurrentState(Gamestate::ID stateOfGame); // Prints to console

	// state functions
	void runState();
	void menuState();
	void levelState();

	char windowName[20];

	void initWindow();
	void initScene(GameScene &scene);
	void clearScene(GameScene &scene);
	void initShaderProgramLib();
	void initInputOptions();

	void useShaderProgram();

	void addMeshName();
	//...
	void addLights(GameScene &scene);
	void addRenderManager(GameScene &scene);
	void addPlayer(GameScene &scene);
	void addMeshFilter(GameScene &scene);
	void readMeshName();
	//...
	void processInput(GLFWwindow *window, float deltaTime, GameScene& scene);

	//Time Variables
	float deltaTime;
	float seconds;
	//
	bool meshesLoaded;
	bool fullscreen;

	int count;

	// bool in processInput for state swapping
	bool stateBool;

};


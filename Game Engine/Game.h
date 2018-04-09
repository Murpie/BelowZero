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
#include "CharacterMovement.h"
#include <string>
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include "glm\glm\glm.hpp"
#include "Light.h"
#include <ctime>
#include "GeometryShaders.h"
#include "SSAOShaders.h"
#include "SSAOBlurShaders.h"
#include "LightpassShaders.h"
#include "FXAAShaders.h"
#include "CubeMapShaders.h"
#include "EnumID.h"

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
	GameScene gameScene; // maybe  vector<GameScene> and switch between gameScene[i].update()
	ShaderProgramLib shaderProgramLibrary;
	MaterialLib materialLibrary;
	TextureLib textureLibrary;
	MeshLib meshLibrary;
	
	//
	vector<Light> lights; // not sure if want to add this here
	vector<CharacterMovement> moveScript;
	vector<RenderManager> renderManager;
	vector<string> meshName;
	vector<MeshFilter> meshFilter;

	Gamestate::ID stateOfGame;
	void printCurrentState(Gamestate::ID stateOfGame);
	void runState();

	char windowName[];

	void initWindow();
	void initShaderProgramLib();

	void addGameObjects();
	void addCamera();
	void addLights();
	void addRenderManager();
	void addCharacterMovement();
	void addMeshName();
	void addMeshFilter();

	void readMeshName();

	void setGameObjects();
};


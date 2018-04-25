#pragma once

#include "Component.h"
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include <GL/gl3w.h>  
#include "ShaderBase.h"
#include "Mesh.h"
#include "stb_image.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

class MainMenuScene : public Component
{
public:
	MainMenuScene();
	~MainMenuScene();

	void checkMousePosFromFunction();
	void setMousePos(glm::vec2 newMousePosition);

	void loadBackgroundTexture(std::string backgroundTextureName);
	void loadButtonTexture(std::string buttonTextureName, int buttonNumber);
	void loadBuffers();
	void createShaders();
	void renderButtons();

	void deleteObjects();

	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);

	GLuint backgroundTexture;
	GLuint startButtonTexture;
	GLuint settingsButtonTexture;
	GLuint quitButtonTexture;

	unsigned int whichButtonIsSelected;
	unsigned int mainMenuShaderProgram;
	unsigned int backgroundFbo;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	unsigned int startButtonFBO;
	unsigned int settingButtonFBO;
	unsigned int ExitButtonFBO;

	glm::vec2 startButtonMinMax[2];
	glm::vec2 settingsButtonMinMax[2];
	glm::vec2 ExitButtonMinMax[2];

	bool mouseIsOverButton;
	bool buttonHasBeenClicked;
	bool buttonHasBeenReleased;

	double xPos;
	double yPos;
private:
	Gamestate::ID stateOfGame;
};
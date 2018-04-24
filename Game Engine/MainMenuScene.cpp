#include "MainMenuScene.h"

MainMenuScene::MainMenuScene()
{
	this-> mouseIsOverButton = false;
	this-> buttonHasBeenClicked = false;
	this-> buttonHasBeenReleased = false;
	this-> whichButtonIsSelected = -1;

	loadMainMenu();
}

MainMenuScene::~MainMenuScene()
{
}

void MainMenuScene::checkMousePosFromFunction()
{
	glfwGetCursorPos(testWindow, &xPos, &yPos);

	this->startButtonMinMax[0].x = 100;
	this->startButtonMinMax[0].y = 50;
	this->startButtonMinMax[1].x = 300;
	this->startButtonMinMax[1].y = 100;

	if (startButtonMinMax[0].x > xPos && xPos < startButtonMinMax[1].x && startButtonMinMax[0].y > yPos && yPos < startButtonMinMax[1].y)
	{
		this->whichButtonIsSelected = 1;
		std::cout << "------------------- CURSOR IS INSIDE STARTBUTTONBOX -------------------" << std::endl;
		this->mouseIsOverButton = true;
		if (glfwGetMouseButton(testWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			this-> buttonHasBeenClicked = true;
			if (glfwGetMouseButton(testWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
			{
				this->buttonHasBeenReleased = true;
			}
		}
	}

	else if (settingsButtonMinMax[0].x > xPos && xPos < settingsButtonMinMax[1].x && settingsButtonMinMax[0].y > yPos && yPos < settingsButtonMinMax[1].y)
	{
		this->whichButtonIsSelected = 2;
		std::cout << "------------------- CURSOR IS INSIDE SETTINGSBUTTONBOX -------------------" << std::endl;
		this->mouseIsOverButton = true;
		if (glfwGetMouseButton(testWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			this->buttonHasBeenClicked = true;
			if (glfwGetMouseButton(testWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
			{
				this->buttonHasBeenReleased = true;
			}
		}
	}

	else if (ExitButtonMinMax[0].x > xPos && xPos < ExitButtonMinMax[1].x && ExitButtonMinMax[0].y > yPos && yPos < ExitButtonMinMax[1].y)
	{
		this->whichButtonIsSelected = 3;
		std::cout << "------------------- CURSOR IS INSIDE EXITBUTTONBOX -------------------" << std::endl;
		this->mouseIsOverButton = true;
		if (glfwGetMouseButton(testWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			this->buttonHasBeenClicked = true;
			if (glfwGetMouseButton(testWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
			{
				this->buttonHasBeenReleased = true;
			}
		}
	}

	if (this->buttonHasBeenClicked == true && this->buttonHasBeenReleased == true)
	{
		if (whichButtonIsSelected == 1)
			stateOfGame = GameState::ID::loadmap;
		else if (whichButtonIsSelected == 2)
			stateOfGame = GameState::ID::loadSettings;
		else if (whichButtonIsSelected == 3)
			stateOfGame = GameState::ID::loadExit;
	}
}

void MainMenuScene::setMousePos(glm::vec2 newMousePosition)
{
}

void MainMenuScene::loadMainMenu()
{
	loadBackgroundTexture("MainMenuBackground");
}

void MainMenuScene::loadBackgroundTexture(std::string backgroundTextureName)
{
	std::string texturePNG = ".png";
	std::string filePath = backgroundTextureName + texturePNG;
	int width, height, nrOfChannels;

	// ----------========== Equipment FrameBuffer ==========----------
	unsigned char * data = stbi_load(filePath.c_str(), &width, &height, &nrOfChannels, 0);

	glGenFramebuffers(1, &backgroundFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, backgroundFbo);

	glGenTextures(1, &backgroundTexture);
	
	glBindTexture(GL_TEXTURE_2D, backgroundTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load Background Texture from path" << std::endl;
	}

	stbi_image_free(data);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backgroundTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Background Framebuffer not complete!" << std::endl;
}


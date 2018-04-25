#include "MainMenuScene.h"

MainMenuScene::MainMenuScene()
{
	this->mouseIsOverButton = false;
	this->buttonHasBeenClicked = false;
	this->buttonHasBeenReleased = false;
	this->whichButtonIsSelected = -1;

	this->startButtonMinMax[0].x = 100;
	this->startButtonMinMax[0].y = 50;
	this->startButtonMinMax[1].x = 300;
	this->startButtonMinMax[1].y = 100;

	loadBackgroundTexture("MainMenuBackground");
	loadBuffers();
	createShaders();
	renderButtons();
}

MainMenuScene::~MainMenuScene()
{
	deleteObjects();
}

void MainMenuScene::checkMousePosFromFunction()
{
	
}

void MainMenuScene::setMousePos(glm::vec2 newMousePosition)
{
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
	
	stbi_set_flip_vertically_on_load(true);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (data)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else
		std::cout << "Failed to load Background Texture from path" << std::endl;

	stbi_image_free(data);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backgroundTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Background Framebuffer not complete!" << std::endl;
}

void MainMenuScene::loadBuffers()
{
	float vertices[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = { 
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MainMenuScene::createShaders()
{
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	std::ifstream shaderFile("ShadowMapVS.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	const char* shaderTextPtr = shaderText.c_str();
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		glGetShaderInfoLog(vs, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("ShadowMapFS.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	mainMenuShaderProgram = glCreateProgram();
	glAttachShader(mainMenuShaderProgram, fs);
	glAttachShader(mainMenuShaderProgram, vs);
	glLinkProgram(mainMenuShaderProgram);


	compileResult = GL_FALSE;
	glGetProgramiv(mainMenuShaderProgram, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		memset(buff, 0, 1024);
		glGetProgramInfoLog(mainMenuShaderProgram, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	glDetachShader(mainMenuShaderProgram, vs);
	glDetachShader(mainMenuShaderProgram, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void MainMenuScene::renderButtons()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(mainMenuShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, startButtonTexture);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void MainMenuScene::deleteObjects()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void MainMenuScene::update(float deltaTime, float seconds)
{

}

void MainMenuScene::processEvents(GLFWwindow * window, float deltaTime)
{
	glfwGetCursorPos(window, &xPos, &yPos);

	if (startButtonMinMax[0].x > xPos && xPos < startButtonMinMax[1].x && startButtonMinMax[0].y > yPos && yPos < startButtonMinMax[1].y)
	{
		this->whichButtonIsSelected = 1;
		std::cout << "------------------- CURSOR IS INSIDE STARTBUTTONBOX -------------------" << std::endl;
		this->mouseIsOverButton = true;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			this->buttonHasBeenClicked = true;
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
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
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			this->buttonHasBeenClicked = true;
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
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
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			this->buttonHasBeenClicked = true;
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
			{
				this->buttonHasBeenReleased = true;
			}
		}
	}

	if (this->buttonHasBeenClicked == true && this->buttonHasBeenReleased == true)
	{
		this->mouseIsOverButton = true;
		this->buttonHasBeenClicked = true;
		this->buttonHasBeenReleased = true;
		if (whichButtonIsSelected == 1)

			stateOfGame = Gamestate::ID::LOAD_LEVEL;
		/*else if (whichButtonIsSelected == 2)
		stateOfGame = GameState::ID::loadSettings;
		else if (whichButtonIsSelected == 3)
		stateOfGame = GameState::ID::loadExit;*/
	}
}


#include "Game.h"

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}


void mouse_enter_callback(GLFWwindow * window, int entered)
{
	if (entered)
		std::cout << "CURSOR::ENTER::WINDOW" << std::endl;
	else
		std::cout << "CURSOR::EXIT::WINDOW" << std::endl;
}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		std::cout << "MOUSEBUTTON::LEFT::PRESS" << std::endl;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		std::cout << "MOUSEBUTTON::LEFT::RELEASE" << std::endl;
	}

	// entity.processMouseEvent(window, button, action);
	//
}

void Game::processInput(GLFWwindow *window, float deltaTime) //GameScene& scene
{
	/* 
		In this function we want to call on the sceneObjects.
		
		example : 
			scene.pollEvent(window, deltaTime);

		and check inside the classes if we want to make something 
		happen depending on which button we press.

		This function should be called on within the correct state in runState().
	*/

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	//------------------------------------
	//This statement should be used inside the GUI class
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		std::cout << "CUROSR::X::POSITION::" << xpos << std::endl;
		std::cout << "CUROSR::Y::POSITION::" << ypos << std::endl;

		//
		//glm::vec3 worldRay = Ray::getWorldRay(xpos, ypos, glm::mat4(), SCREEN_WIDTH, SCREEN_HEIGHT);
		//std::cout << "CUROSR::WORLDRAY::" << worldRay.x << " " << worldRay.y << " " << worldRay.z << std::endl;

	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{

	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		stateOfGame = Gamestate::ID::RUN_LEVEL;
		printCurrentState(stateOfGame);
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		stateOfGame = Gamestate::ID::SHOW_MENU;
		printCurrentState(stateOfGame);
	}
	// Add this inside the player class, processEvents() function for movement etc. 
	/*
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{}
	*/
}

Game::Game() :
	shaderProgramLibrary(),
	gameScene(), menuScene(),
	windowName("Game Engine"),
	stateOfGame(Gamestate::ID::INITIALIZE),
	deltaTime(0), seconds(0),
	gaussianblur(false), fxaa(false), ssao(true), meshesLoaded(false), testBool(false)
{
	initWindow();
	initShaderProgramLib();
	addMeshName();
}

Game::~Game()
{

}

void Game::run()
{
	printCurrentState(stateOfGame);
	//Render
	auto startSeconds = chrono::high_resolution_clock::now();
	auto startDeltaTime = chrono::high_resolution_clock::now();

	int initial_time = time(NULL);
	int final_time;
	int frameCount = 0;
	
	initScene(menuScene);
	initScene(gameScene);

	useShaderProgram();

	stateOfGame = Gamestate::ID::RUN_LEVEL;
	printCurrentState(stateOfGame);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		//float deltaTime;
		auto nowDeltaTime = chrono::high_resolution_clock::now();
		deltaTime = chrono::duration_cast<chrono::duration<float>>(nowDeltaTime - startDeltaTime).count();
		nowDeltaTime = startDeltaTime;

		float secondsTime;
		auto nowSeconds = chrono::high_resolution_clock::now();
		seconds = (float)chrono::duration_cast<std::chrono::milliseconds>(nowSeconds - startSeconds).count();
		//nowSeconds = startSeconds;

		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		runState();
		processInput(window, deltaTime); //...

		glfwSwapBuffers(window);

		frameCount++;
		final_time = time(NULL);
		if (final_time - initial_time > 0)
		{
			sprintf(windowName, "Game Engine FPS : %d", frameCount / (final_time - initial_time));
			glfwSetWindowTitle(window, windowName);
			frameCount = 0;
			initial_time = final_time;
		}
	}
	glfwTerminate();
}


void Game::printCurrentState(Gamestate::ID stateOfGame)
{
	std::cout << "CURRENT_GAMESTATE:: " << stateOfGame << std::endl;
}

void Game::runState()
{
	//Add a main state and seperate the function in substates or keep it as it is?
	/*
		main-state:		sub-states :
		MENU_STATE	->	CLEAR, LOAD, RUN 
		LEVEL_STATE ->	CLEAR, LOAD, RUN
	*/
	if (stateOfGame == Gamestate::ID::SHOW_MENU)
	{
		menuScene.update(deltaTime);
		//processInput(window, deltaTime);
		renderManager[0].setDeltaTime(deltaTime);
		renderManager[0].setSeconds(seconds);
		renderManager[0].Render(ssao);
	}
	else if (stateOfGame == Gamestate::ID::RUN_LEVEL)
	{
		//gameScenes[1].update(deltaTime);
		gameScene.update(deltaTime);
		//processInput(window, deltaTime);
		renderManager[1].setDeltaTime(deltaTime);
		renderManager[1].setSeconds(seconds);
		renderManager[1].Render(ssao);
	}
	else {
		return;
	}
}

void Game::initWindow()
{
	// Setup window
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	window = glfwCreateWindow(1280, 720, windowName, NULL, NULL);
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1); // Enable vsync
	gl3wInit();
}

void Game::initScene(GameScene & scene)
{
	addRenderManager(scene); // return int and set a variable inside the gamescene and use that number when updating in states. 
	//... Create Camera
	addCharacterMovement(scene);
	//... Create Lights
	addLights(scene);
	//... Read OBJ and MTL File
	if(!meshesLoaded)
	{ 
		//Load the meshes once and store them.
		readMeshName();	
		meshesLoaded = true;
	}
	//...
	/*Add meshes to mesh filter with level file ?*/
	addMeshFilter(scene);
}

void Game::initShaderProgramLib()
{
	shaderProgramLibrary.addGeometryPassShaders();
	shaderProgramLibrary.addCubeMapShaders();
	shaderProgramLibrary.addSSAOShaders();
	shaderProgramLibrary.addBlurShaders();
	shaderProgramLibrary.addLightpassShaders();
	shaderProgramLibrary.addSkyboxShaders();
	shaderProgramLibrary.addGaussianBlurShaders();
	shaderProgramLibrary.addFXAAShaders();
	shaderProgramLibrary.addShadowMapShaders();
	shaderProgramLibrary.addPointLightShadowMapShaders();
	shaderProgramLibrary.addAnimationShaders();
}

void Game::initInputOptions()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorEnterCallback(window, mouse_enter_callback);	//? Needed to check when inside and outside of window
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
}

void Game::useShaderProgram()
{
	glUseProgram(shaderProgramLibrary.getShader<GaussianBlurShaders>()->gaussianBlurShaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgramLibrary.getShader<GaussianBlurShaders>()->gaussianBlurShaderProgram, "onOrOff"), gaussianblur);

	glUseProgram(shaderProgramLibrary.getShader<FXAAShaders>()->fxaaShaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgramLibrary.getShader<FXAAShaders>()->fxaaShaderProgram, "swap"), fxaa);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);
}

void Game::addMeshName()
{
	//Add file names to vector to load when reading mesh data. 

	std::string tempMeshName;

	tempMeshName = "Floor.obj";
	meshName.push_back(tempMeshName);

	tempMeshName = "House2.obj";
	meshName.push_back(tempMeshName);

	tempMeshName = "House1.obj";
	meshName.push_back(tempMeshName);
}

void Game::addLights(GameScene &scene)
{
	// add for loop and use array for transforms;
	if (!testBool)
	{
		scene.addLight(glm::vec3(7, 9, -4), 0);
		scene.addLight(glm::vec3(4, 0.4, -2), 1);
		testBool = true;
	}
	else
	{
		scene.gameObjects[0].transform = glm::vec3(4, 0.4, -2);
	}	
}

void Game::addRenderManager(GameScene &scene)
{
	RenderManager tempRender = RenderManager(&scene, window, &shaderProgramLibrary);
	renderManager.push_back(tempRender);
}

void Game::addCharacterMovement(GameScene &scene)
{
	scene.addCharacterMovement(window);
}

void Game::addMeshFilter(GameScene &scene)
{
	scene.addMeshFilter(meshLibrary, materialLibrary, meshName.size());
}

void Game::readMeshName()
{
	for (int i = 0; i < meshName.size(); i++)
	{
		//scene.addEmptyGameObject();
		meshLibrary.addMesh(meshName[i], shaderProgramLibrary.getShader<GeometryShaders>()->geometryShaderProgram);
		//Add material
		materialLibrary.addMaterial(shaderProgramLibrary.getShader<GeometryShaders>()->geometryShaderProgram);

		//If found variables are not querried, Set standard values in shader.
		//1: send in texture
		//2: send in mtl values
		unsigned int foundAlbedo = 0;
		unsigned int foundNormal = 0;
		unsigned int foundSpecular = 0;
		unsigned int foundMetallic = 0;
		unsigned int foundAO = 0;
		glm::mat3 mtlInfo;

		const char *meshNameChar = meshName[i].c_str();
		FILE *file = fopen(meshNameChar, "r");
		while (file != NULL)
		{
			char lineHeader[128];
			int word = fscanf(file, "%s", lineHeader);
			if (word == EOF) //EOF = End Of Line.
			{
				materialLibrary.getMaterial(i)->addFoundAlbedo(foundAlbedo);
				materialLibrary.getMaterial(i)->addFoundNormal(foundNormal);
				materialLibrary.getMaterial(i)->addFoundSpecular(foundSpecular);
				materialLibrary.getMaterial(i)->addFoundMetallic(foundMetallic);
				materialLibrary.getMaterial(i)->addFoundAO(foundAO);
				materialLibrary.getMaterial(i)->addMtlInfo(mtlInfo);

				foundAlbedo = 0;
				foundSpecular = 0;
				foundNormal = 0;
				foundMetallic = 0;
				foundAO = 0;
				break;
			}
			else if (strcmp(lineHeader, "mtllib") == 0)
			{

				char mtlChar[128];
				fscanf(file, "%s", mtlChar);
				FILE* mtlFile = fopen(mtlChar, "r");

				while (true)
				{
					int textureLocation = 0;
					char mtlLineHeader[128];
					int mtlWord = fscanf(mtlFile, "%s", mtlLineHeader);
					if (mtlWord == EOF)
					{
						break;
					}
					else if (strcmp(mtlLineHeader, "Kd") == 0)
					{
						fscanf(mtlFile, "%f %f %f", &mtlInfo[0].x, &mtlInfo[0].y, &mtlInfo[0].z);
						foundAlbedo = 2;
					}
					else if (strcmp(mtlLineHeader, "Ks") == 0)
					{
						fscanf(mtlFile, "%f %f %f", &mtlInfo[1].x, &mtlInfo[1].y, &mtlInfo[1].z);
						foundSpecular = 2;
					}
					else if (strcmp(mtlLineHeader, "map_Kd") == 0)
					{
						char textureFile[128];
						fscanf(mtlFile, "%s", textureFile);
						textureLibrary.addAlbedo(textureFile);
						if (textureLibrary.albedo.size() - 1 != i)
						{
							for (int j = i; j > 0; j--)
							{
								if (j == textureLibrary.albedo.size() - 1)
								{
									textureLocation = j;
									break;
								}
							}
						}
						else
						{
							textureLocation = i;
						}
						materialLibrary.getMaterial(i)->addAlbedo(textureLibrary.getAlbedo(textureLocation)->gTexture);
						foundAlbedo = 1;
					}
					else if (strcmp(mtlLineHeader, "map_Ks") == 0)
					{
						char textureFile[128];
						fscanf(mtlFile, "%s", textureFile);
						textureLibrary.addSpecular(textureFile);
						if (textureLibrary.specular.size() - 1 != i)
						{
							for (int j = i; j > 0; j--)
							{
								if (j == textureLibrary.specular.size() - 1)
								{
									textureLocation = j;
									break;
								}
							}
						}
						else
						{
							textureLocation = i;
						}
						materialLibrary.getMaterial(i)->addSpecular(textureLibrary.getSpecular(textureLocation)->gTexture);
						foundSpecular = 1;
					}
					else if (strcmp(mtlLineHeader, "map_Normal") == 0)
					{
						char textureFile[128];
						fscanf(mtlFile, "%s", textureFile);
						textureLibrary.addNormal(textureFile);
						if (textureLibrary.normal.size() - 1 != i)
						{
							for (int j = i; j > 0; j--)
							{
								if (j == textureLibrary.normal.size() - 1)
								{
									textureLocation = j;
									break;
								}
							}
						}
						else
						{
							textureLocation = i;
						}
						materialLibrary.getMaterial(i)->addNormal(textureLibrary.getNormal(textureLocation)->gTexture);
						foundNormal = 1;
					}
					else if (strcmp(mtlLineHeader, "map_Metallic") == 0)
					{
						char textureFile[128];
						fscanf(mtlFile, "%s", textureFile);
						textureLibrary.addMetallic(textureFile);
						if (textureLibrary.metallic.size() - 1 != i)
						{
							for (int j = i; j > 0; j--)
							{
								if (j == textureLibrary.metallic.size() - 1)
								{
									textureLocation = j;
									break;
								}
							}
						}
						else
						{
							textureLocation = i;
						}
						materialLibrary.getMaterial(i)->addMetallic(textureLibrary.getMetallic(textureLocation)->gTexture);
						foundMetallic = 1;
					}
					else if (strcmp(mtlLineHeader, "map_AO") == 0)
					{
						char textureFile[128];
						fscanf(mtlFile, "%s", textureFile);
						textureLibrary.addAO(textureFile);
						if (textureLibrary.ao.size() - 1 != i)
						{
							for (int j = i; j > 0; j--)
							{
								if (j == textureLibrary.ao.size() - 1)
								{
									textureLocation = j;
									break;
								}
							}
						}
						else
						{
							textureLocation = i;
						}
						materialLibrary.getMaterial(i)->addAO(textureLibrary.getAO(textureLocation)->gTexture);
						foundAO = 1;
					}
				}
			}
		}
	}
}






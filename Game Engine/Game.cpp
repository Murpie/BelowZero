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

void Game::processInput(GLFWwindow *window, float deltaTime, GameScene& scene) //GameScene& scene
{
	if (glfwGetKey(window, GLFW_KEY_F5) && !fullscreen)
	{
		glfwSetWindowMonitor(window, primary[0], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mode->refreshRate);
		fullscreen = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_F5) && fullscreen)
	{
		glfwSetWindowMonitor(window, 0, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, mode->refreshRate);
		fullscreen = false;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && stateBool != true)
	{
		stateBool = true;
		if(stateOfGame == Gamestate::ID::RUN_LEVEL)
			stateOfGame = Gamestate::ID::CLEAR_LEVEL;
		else if (stateOfGame == Gamestate::ID::SHOW_MENU)
			stateOfGame = Gamestate::ID::CLEAR_MENU;
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && stateBool != false)
	{
		stateBool = false;
	}
	//...
	scene.processEvents(window, deltaTime);
}

Game::Game() :
	shaderProgramLibrary(),
	gameScene(Scene::ID::LEVEL_1), menuScene(Scene::ID::MENU),
	windowName("Game Engine"),
	stateOfGame(Gamestate::ID::INITIALIZE),
	deltaTime(0), seconds(0),
	meshesLoaded(false), fullscreen(false), stateBool(false), texturesLoaded(false),
	count(0)
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

	clock_t begin = clock();

	int initial_time = time(NULL);
	int final_time;
	int frameCount = 0;

	useShaderProgram();

	stateOfGame = Gamestate::ID::LOAD_MENU;
	printCurrentState(stateOfGame);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		clock_t end = clock();
		deltaTime = float(end - begin) / CLOCKS_PER_SEC;
		begin = end;

		/*	auto nowDeltaTime = chrono::high_resolution_clock::now();
		deltaTime = chrono::duration_cast<chrono::duration<float>>(nowDeltaTime - startDeltaTime).count() / 1000;
		startDeltaTime = nowDeltaTime;*/

		float secondsTime;
		auto nowSeconds = chrono::high_resolution_clock::now();
		seconds = (float)chrono::duration_cast<std::chrono::milliseconds>(nowSeconds - startSeconds).count();
		startSeconds = nowSeconds;

		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		runState();
		//...

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
	clearScene(menuScene);
	clearScene(gameScene);

	for (int i = 0; i < renderManager.size(); i++)
	{
		renderManager[i].deleteData();
	}
	renderManager.clear();

	glfwTerminate();
}

void Game::printCurrentState(Gamestate::ID stateOfGame)
{
	std::cout << "CURRENT_GAMESTATE:: " << stateOfGame << std::endl;
}

void Game::runState()
{
	//... Menu
	if (stateOfGame == Gamestate::ID::LOAD_MENU || stateOfGame == Gamestate::ID::SHOW_MENU || stateOfGame == Gamestate::ID::CLEAR_MENU)
	{
		menuState();
	}
	//... Level
	else if (stateOfGame == Gamestate::ID::LOAD_LEVEL || stateOfGame == Gamestate::ID::RUN_LEVEL || stateOfGame == Gamestate::ID::CLEAR_LEVEL)
	{
		levelState();
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwDisable(GLFW_MOUSE_CURSOR);
	}
	//... 
	else
	{
		return;
	}
}

void Game::menuState()
{
	if (stateOfGame == Gamestate::ID::LOAD_MENU)
	{
		printCurrentState(stateOfGame);
		initScene(menuScene);
		stateOfGame = Gamestate::ID::SHOW_MENU;
		printCurrentState(stateOfGame);
	}
	else if (stateOfGame == Gamestate::ID::SHOW_MENU)
	{
		menuScene.update(deltaTime, seconds);
		processInput(window, deltaTime, menuScene);
		renderManager[0].setDeltaTime(deltaTime);
		renderManager[0].setSeconds(seconds);
		renderManager[0].Render();
	}
	else if (stateOfGame == Gamestate::ID::CLEAR_MENU)
	{
		clearScene(menuScene);
		stateOfGame = Gamestate::ID::LOAD_LEVEL;
	}
}

void Game::levelState()
{
	if (stateOfGame == Gamestate::ID::LOAD_LEVEL)
	{
		printCurrentState(stateOfGame);
		initScene(gameScene);
		stateOfGame = Gamestate::ID::RUN_LEVEL;
		printCurrentState(stateOfGame);
	}
	else if (stateOfGame == Gamestate::ID::RUN_LEVEL)
	{
		gameScene.update(deltaTime, seconds);
		processInput(window, deltaTime, gameScene);
		renderManager[1].setDeltaTime(deltaTime);
		renderManager[1].setSeconds(seconds);
		renderManager[1].Render();
	}
	else if (stateOfGame == Gamestate::ID::CLEAR_LEVEL)
	{
		printCurrentState(stateOfGame);
		clearScene(gameScene);
		stateOfGame = Gamestate::ID::LOAD_MENU;
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
	primary = glfwGetMonitors(&count);
	mode = glfwGetVideoMode(primary[0]);
	window = glfwCreateWindow(1280, 720, windowName, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	gl3wInit();
}

void Game::initScene(GameScene & scene)
{
	// RenderManager probably needs a rework when loaded with scene. 
	if(renderManager.size() < 2)
		addRenderManager(scene); // return int and set a variable inside the gamescene and use that number when updating in states. 
	//... Read OBJ and MTL File
	if (!meshesLoaded)
	{
		//Load the meshes once and store them.
		readMeshName(scene);
		meshesLoaded = true;
	}
	//...
	scene.initScene(meshLibrary, materialLibrary, shaderProgramLibrary, scene.typeOfScene);
}

void Game::clearScene(GameScene & scene)
{
	scene.clearGameObjects();
	//renderManager.clear();
	/* Add function to also clear the renderManager*/
}

void Game::initShaderProgramLib()
{
	shaderProgramLibrary.addGeometryPassShaders();
	shaderProgramLibrary.addLightpassShaders();
	shaderProgramLibrary.addShadowMapShaders();
	shaderProgramLibrary.addUIShaders();
	shaderProgramLibrary.addVFXShaders();
	shaderProgramLibrary.addTerrainShaders();
}

void Game::initInputOptions()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorEnterCallback(window, mouse_enter_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
}

void Game::useShaderProgram()
{
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);
}

void Game::addMeshName()
{
	//Add file names to vector to load when reading mesh data. 
	//std::string meshLoader[] = { "Stone.leap", "Bucket.leap", "Stump.leap", "Tree.leap", "TreeWithSnow.leap", "Floor.leap" };
	std::string meshLoader[] = { "Player_temp.leap", "Bucket.leap", "Stone_1.leap", "Campfire.leap"};

	for (int i = 0; i < sizeof(meshLoader) / sizeof(meshLoader[0]); i++)
	{
		meshName.push_back(meshLoader[i]);
	}
}

void Game::addRenderManager(GameScene &scene)
{
	RenderManager tempRender = RenderManager(&scene, window, &shaderProgramLibrary);
	renderManager.push_back(tempRender);
}

void Game::readMeshName(GameScene &scene)
{
	for (int i = 0; i < meshName.size(); i++)
	{
		meshLibrary.addMesh(meshName[i], shaderProgramLibrary.getShader<GeometryShaders>()->geometryShaderProgram);
	}
	if (!texturesLoaded)
	{
		materialLibrary.addMaterial(shaderProgramLibrary.getShader<GeometryShaders>()->geometryShaderProgram);
		textureLibrary.addAlbedo("Colors.png");
		materialLibrary.getMaterial(0)->addAlbedo(textureLibrary.getAlbedo(0)->gTexture);
		texturesLoaded = true;
	}
}
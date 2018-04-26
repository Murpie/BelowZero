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
		glfwSetWindowMonitor(window, primary[0], 0, 0, 1280, 720, mode->refreshRate);
		fullscreen = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_F5) && fullscreen)
	{
		glfwSetWindowMonitor(window, 0, 100, 100, 1280, 720, mode->refreshRate);
		fullscreen = false;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GL_TRUE);
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
	/*
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
	*/
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && stateBool != true)
	{
		stateBool = true;
		if(stateOfGame.state == Gamestate::ID::RUN_LEVEL)
			stateOfGame.state = Gamestate::ID::CLEAR_LEVEL;
		else if (stateOfGame.state == Gamestate::ID::SHOW_MENU)
			stateOfGame.state = Gamestate::ID::CLEAR_MENU;
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
	gameScene(), menuScene(),
	windowName("Game Engine"),
	deltaTime(0), seconds(0),
	meshesLoaded(false), fullscreen(false), stateBool(false),
	count(0)
{
	initWindow();
	initShaderProgramLib();
	addMeshName();
	stateOfGame.state = Gamestate::ID::INITIALIZE;
}

Game::~Game()
{

}

void Game::run()
{
	printCurrentState(stateOfGame.state);
	//Render
	auto startSeconds = chrono::high_resolution_clock::now();

	clock_t begin = clock();

	int initial_time = time(NULL);
	int final_time;
	int frameCount = 0;

	useShaderProgram();

	stateOfGame.state = Gamestate::ID::LOAD_MENU;
	printCurrentState(stateOfGame.state);

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
	glfwTerminate();
}

void Game::printCurrentState(Gamestate::ID stateOfGame)
{
	std::cout << "CURRENT_GAMESTATE:: " << stateOfGame << std::endl;
}

void Game::runState()
{
	//... Menu
	if (stateOfGame.state == Gamestate::ID::LOAD_MENU || stateOfGame.state == Gamestate::ID::SHOW_MENU || stateOfGame.state == Gamestate::ID::CLEAR_MENU)
	{
		menuState();
	}
	//... Level
	else if (stateOfGame.state == Gamestate::ID::LOAD_LEVEL || stateOfGame.state == Gamestate::ID::RUN_LEVEL || stateOfGame.state == Gamestate::ID::CLEAR_LEVEL)
	{
		levelState();
	}
	else if (stateOfGame.state == Gamestate::ID::CLOSE_GAME)
	{
		exitState();
	}
	//... 
	else
	{
		return;
	}
}

void Game::menuState()
{
	if (stateOfGame.state == Gamestate::ID::LOAD_MENU)
	{
		printCurrentState(stateOfGame.state);
		//initScene(menuScene);
		initMenuScene(menuScene);
		stateOfGame.state = Gamestate::ID::SHOW_MENU;
		printCurrentState(stateOfGame.state);
	}
	else if (stateOfGame.state == Gamestate::ID::SHOW_MENU)
	{
		menuScene.update(deltaTime, seconds);
		processInput(window, deltaTime, menuScene);
		renderManager[0].setDeltaTime(deltaTime);
		renderManager[0].setSeconds(seconds);
		renderManager[0].renderMainMenu();
	}
	else if (stateOfGame.state == Gamestate::ID::CLEAR_MENU)
	{
		clearScene(menuScene);
		stateOfGame.state = Gamestate::ID::LOAD_LEVEL;
	}
}

void Game::levelState()
{
	if (stateOfGame.state == Gamestate::ID::LOAD_LEVEL)
	{
		printCurrentState(stateOfGame.state);
		initScene(gameScene);
		stateOfGame.state = Gamestate::ID::RUN_LEVEL;
		printCurrentState(stateOfGame.state);
	}
	else if (stateOfGame.state == Gamestate::ID::RUN_LEVEL)
	{
		gameScene.update(deltaTime, seconds);
		processInput(window, deltaTime, gameScene);
		renderManager[1].setDeltaTime(deltaTime);
		renderManager[1].setSeconds(seconds);
		renderManager[1].Render();
	}
	else if (stateOfGame.state == Gamestate::ID::CLEAR_LEVEL)
	{
		printCurrentState(stateOfGame.state);
		clearScene(gameScene);
		//stateOfGame = Gamestate::ID::LOAD_MENU;
		stateOfGame.state = Gamestate::ID::LOAD_MENU;
	}
}

void Game::exitState()
{
	clearScene(menuScene);
	renderManager.clear();
	glfwSetWindowShouldClose(window, GL_TRUE);
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
	if (renderManager.size() < 2)
	{
		addRenderManager(scene); // return int and set a variable inside the gamescene and use that number when updating in states. 
		renderManager[renderManager.size() - 1].createBuffers();
	}
	//... Create Camera
	addPlayer(scene);
	addTerrain(scene);
	//... Create Lights
	addLights(scene);
	//... Read OBJ and MTL File
	if (!meshesLoaded)
	{
		//Load the meshes once and store them.
		readMeshName(scene);
		meshesLoaded = true;
	}
	//...
	/*Add meshes to mesh filter with level file ?*/
	addMeshFilter(scene);
}

void Game::initMenuScene(GameScene & scene)
{
	if (renderManager.size() < 2)
	{
		addRenderManager(scene);
		renderManager[renderManager.size() - 1].createMainMenuBuffer();
	}
	//addMeshFilter(scene);
	scene.addMainMenu();
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
	//shaderProgramLibrary.addAnimationShaders();
	shaderProgramLibrary.addUIShaders();
	shaderProgramLibrary.addTerrainShaders();
	shaderProgramLibrary.addMainMenuShaders();
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
	std::string meshLoader[] = { "Bucket.leap", "Stone_1.leap"};
	//meshType: 0 = Static  2 = Interactive  3 = Equiped
	GLuint meshTypes[] = { 0 };

	for (int i = 0; i < sizeof(meshLoader) / sizeof(meshLoader[0]); i++)
	{
		meshName.push_back(meshLoader[i]);
		meshType.push_back(meshTypes[i]);
	}
}

void Game::addLights(GameScene &scene)
{
	// add for loop and use array for transforms ?
	scene.addLight(glm::vec3(-4, 2, 0), 0);
	scene.addLight(glm::vec3(-4, 2, 0), 0);
}

void Game::addRenderManager(GameScene &scene)
{
	RenderManager tempRender = RenderManager(&scene, window, &shaderProgramLibrary);
	renderManager.push_back(tempRender);
	//renderManager[renderManager.size() - 1].createBuffers();
}

void Game::addPlayer(GameScene &scene)
{
	scene.addPlayer();
}

void Game::addMeshFilter(GameScene &scene)
{
	scene.addMeshFilter(meshLibrary, materialLibrary, meshName.size());
}
void Game::addTerrain(GameScene &scene)
{
	scene.addTerrain("firstheightmap.jpg", shaderProgramLibrary.getShader<TerrainShaders>()->TerrainShaderProgram);
}

void Game::readMeshName(GameScene &scene)
{
	for (int i = 0; i < meshName.size(); i++)
	{
		meshLibrary.addMesh(meshName[i], shaderProgramLibrary.getShader<GeometryShaders>()->geometryShaderProgram, meshType[i], scene.getTerrainPointer());
	}

	materialLibrary.addMaterial(shaderProgramLibrary.getShader<GeometryShaders>()->geometryShaderProgram);
	textureLibrary.addAlbedo("Colors.png");
	materialLibrary.getMaterial(0)->addAlbedo(textureLibrary.getAlbedo(0)->gTexture);
}
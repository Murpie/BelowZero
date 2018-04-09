#include "Game.h"

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}


Game::Game() :
	shaderProgramLibrary(),
	gameScene(),
	windowName("Game Engine")
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
	//Render
	auto startSeconds = chrono::high_resolution_clock::now();
	auto startDeltaTime = chrono::high_resolution_clock::now();

	bool gaussianblur = false;
	bool fxaa = false;
	bool ssao = true;

	int initial_time = time(NULL);
	int final_time;
	int frameCount = 0;
	
	addRenderManager();
	//... Create Camera and add empty game object
	addCharacterMovement();
	//... Create Lights and add empty game object
	addLights();
	//... Read OBJ and MTL File
	readMeshName();

	//... Set Game Objects, this should be automated by a function when reading from level file
	gameScene.gameObjects[0].name = "Camera";
	gameScene.gameObjects[0].addComponent(&moveScript[0]);

	gameScene.gameObjects[1].name = "Light 1";
	gameScene.gameObjects[1].addComponent(&lights[0]);
	gameScene.gameObjects[1].transform = glm::vec3(7, 9, -4);
	gameScene.gameObjects[1].lightComponent->lightType = 0;

	gameScene.gameObjects[2].name = "Light 2";
	gameScene.gameObjects[2].addComponent(&lights[1]);
	gameScene.gameObjects[2].transform = glm::vec3(4, 0.4, -2);
	gameScene.gameObjects[2].lightComponent->lightType = 1;
	//...
	addMeshFilter();

	//... Uniform in int that tells gaussian to be turned off
	glUseProgram(shaderProgramLibrary.getShader<GaussianBlurShaders>()->gaussianBlurShaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgramLibrary.getShader<GaussianBlurShaders>()->gaussianBlurShaderProgram, "onOrOff"), gaussianblur);

	glUseProgram(shaderProgramLibrary.getShader<FXAAShaders>()->fxaaShaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgramLibrary.getShader<FXAAShaders>()->fxaaShaderProgram, "swap"), fxaa);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	//////////////
	int gameObject_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.

												// Main loop
	while (!glfwWindowShouldClose(window))
	{
		float deltaTime;
		auto nowDeltaTime = chrono::high_resolution_clock::now();
		deltaTime = chrono::duration_cast<chrono::duration<float>>(nowDeltaTime - startDeltaTime).count();
		nowDeltaTime = startDeltaTime;

		float secondsTime;
		auto nowSeconds = chrono::high_resolution_clock::now();
		float seconds = (float)chrono::duration_cast<std::chrono::milliseconds>(nowSeconds - startSeconds).count();
		nowSeconds = startSeconds;


		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		gameScene.update();

		renderManager[0].getDeltaTime(deltaTime);
		renderManager[0].getSeconds(seconds);
		renderManager[0].Render(ssao);

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

void Game::addGameObjects()
{
	addCamera();
	addLights();
}

void Game::addCamera()
{
	//CharacterMovement moveScript = CharacterMovement(window);
	//moveScript = moveScript(window);
	//gameScene.addEmptyGameObject();
	//gameScene.gameObjects[0].name = "Camera"; //+std::to_string(i);
	//gameScene.gameObjects[0].addComponent(&moveScript);
}

void Game::addLights()
{
	int numberOfLights = 2;
	for (int i = 0; i < numberOfLights; i++)
	{
		gameScene.addEmptyGameObject();
		Light light = Light();
		lights.push_back(light);
	}
}

void Game::addRenderManager()
{
	RenderManager tempRender = RenderManager(&gameScene, window, &shaderProgramLibrary);
	renderManager.push_back(tempRender);
}

void Game::addCharacterMovement()
{
	gameScene.addEmptyGameObject();
	CharacterMovement tempMoveScript = CharacterMovement(window);
	moveScript.push_back(tempMoveScript);
}

void Game::addMeshName()
{
	std::string tempMeshName;

	tempMeshName = "Floor.obj";
	meshName.push_back(tempMeshName);

	tempMeshName = "House2.obj";
	meshName.push_back(tempMeshName);

	tempMeshName = "House1.obj";
	meshName.push_back(tempMeshName);
}

void Game::addMeshFilter()
{
	for (int i = 0; i < meshName.size(); i++)
	{
		MeshFilter meshFilterTemp = MeshFilter(meshLibrary.getMesh(i).gVertexBuffer, meshLibrary.getMesh(i).gVertexAttribute, meshLibrary.getMesh(i).gElementBuffer, meshLibrary.getMesh(i).vertexCount);
		meshFilter.push_back(meshFilterTemp);
	}
	for (int i = 0; i < meshName.size(); i++)
	{
		gameScene.gameObjects[i + 3].name = meshName[i];
		gameScene.gameObjects[i + 3].addComponent(&meshFilter[i]);
		gameScene.gameObjects[i + 3].addComponent(materialLibrary.getMaterial(i));
	}
}

void Game::readMeshName()
{
	for (int i = 0; i < meshName.size(); i++)
	{
		gameScene.addEmptyGameObject();
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



#include "GameScene.h"

GameScene::GameScene(Scene::ID typeOfScene) :
	camerasInScene(0), lightsInScene(0)
{
	this->typeOfScene = typeOfScene;
}

GameScene::~GameScene()
{
	clearGameObjects();
}

void GameScene::clearGameObjects()
{
	for (GameObject* gameObject_ptr : gameObjects)
		delete gameObject_ptr;
	gameObjects.clear();
}

void GameScene::update(float deltaTime, float seconds)
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{

		if (gameObjects[i]->getPlayer() != nullptr)
		{
			for (int j = 0; j < gameObjects.size(); j++)
			{
				glm::vec2 UVS = gameObjects[i]->getPlayer()->setXZ();
				float u = UVS.x;
				float v = UVS.y;
				if (gameObjects[j]->getTerrain() != nullptr)
				{
					gameObjects[i]->getPlayer()->setCurrentHeight(gameObjects[j]->getTerrain()->calculateY(u, v));
				}
			}
		}
		gameObjects[i]->update(deltaTime, seconds);
		collisionTest(*gameObjects[i]);
	}
}

void GameScene::processEvents(GLFWwindow * window, float deltaTime)
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->processEvents(window, deltaTime);
		interactionTest(*gameObjects[i], window);
	}
}

void GameScene::initScene(MeshLib & meshLibrary, MaterialLib & matertialLibrary, ShaderProgramLib & shader, Scene::ID typeOfScene)
{
	if (typeOfScene == Scene::ID::LEVEL_1)
	{
		// Camera - (modify position with level file?)
		addPlayer();
		// Lights - (add lights with level file?)
		addLight(glm::vec3(7, 9, -4), 0);
		addLight(glm::vec3(4, 0.4, -2), 1);
		// Terrain
		std::string heightMap = "test1234.jpg";
		addTerrain(heightMap, shader.getShader<TerrainShaders>()->TerrainShaderProgram);
		// Read from level file and add level objects to scene
		LeapLevel* level = new LeapLevel("Level_test.leap");
		addLevelObjects(meshLibrary, matertialLibrary, level);
		delete level;
	}
	else if (typeOfScene == Scene::ID::MENU)
	{
		// Put menu specific scene in here. 
		//...
		addMainMenu();

		std::string heightMap = "test1234.jpg";
		addTerrain(heightMap, shader.getShader<TerrainShaders>()->TerrainShaderProgram);

		LeapLevel* level = new LeapLevel("Level_test.leap");
		addLevelObjects(meshLibrary, matertialLibrary, level);
		delete level;
	}
	else
	{
		std::cout << "GAMESCENE::initScene() - Scene::ID : not valid" << std::endl;
	}
}

void GameScene::addLight(glm::vec3 transform, int lightType)
{
	//Create light object
	GameObject* lightObject = new GameObject();
	lightObject->name = "Light " + std::to_string(lightsInScene);
	lightObject->transform->position = transform;
	//Add light component
	Light* light = new Light(*lightObject->transform);
	light->lightType = lightType;
	lightObject->addComponent(light);
	//Add to scene
	gameObjects.push_back(lightObject);
	lightsInScene++;
}

void GameScene::addPlayer()
{
	//Create player object
	GameObject* playerObject = new GameObject();
	playerObject->transform->position = glm::vec3(0.f, 0.f, 0.f);
	playerObject->objectID = ObjectType::ID::PLAYER;
	playerObject->name = "Player " + camerasInScene;
	//Add player component
	Player* player = new Player(*playerObject->transform);
	playerObject->addComponent(player);
	//Add to scene
	gameObjects.push_back(playerObject);
	camerasInScene++;
}

void GameScene::addLevelObjects(MeshLib & meshLibrary, MaterialLib& materialLibrary, LeapLevel* level)
{
	//Find terrain object
	Terrain* terrain;
	for (GameObject* gameObject_ptr : gameObjects)
	{
		if (gameObject_ptr->getTerrain() != nullptr)
		{
			terrain = gameObject_ptr->getTerrain();
			break;
		}
	}
	//Create game objects from level file
	for (int i = 0; i < level->levelObjects.size(); i++)
	{
		std::cout << "LEVELOBJECT::ID::" << level->levelObjects[i]->id << std::endl;
		if (level->levelObjects[i]->id == ObjectType::ID::PLAYER)
		{
			for (GameObject* gameObject_ptr : gameObjects)
			{
				// Find player object
				if (gameObject_ptr->getPlayer() != nullptr)
				{
					//Add new mesh component to player with data from mesh library
					MeshFilter* meshFilter = new MeshFilter(
						meshLibrary.getMesh(level->levelObjects[i]->id)->gVertexBuffer,
						meshLibrary.getMesh(level->levelObjects[i]->id)->gVertexAttribute,
						meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->getVertexCount(),
						meshLibrary.getMesh(level->levelObjects[i]->id)->meshType);
					gameObject_ptr->addComponent(meshFilter);
					//Set player object position in world
					gameObject_ptr->transform->position = glm::vec3(level->levelObjects[i]->x + 20, level->levelObjects[i]->y, level->levelObjects[i]->z + 20);
					gameObject_ptr->transform->rotation = glm::vec3(level->levelObjects[i]->rotationX, level->levelObjects[i]->rotationY, level->levelObjects[i]->rotationZ);
					//Calculate new world Y-position from height map and update value
					float newPositionY = terrain->calculateY(gameObject_ptr->transform->position.x + 20, gameObject_ptr->transform->position.z + 20) - 2;
					gameObject_ptr->transform->position.y = newPositionY;
					//Add material to gameObject from materialLibrary
					gameObject_ptr->addComponent(materialLibrary.getMaterial(0));
					//Set customAttribute ID from Enum.H
					gameObject_ptr->objectID = (ObjectType::ID)level->levelObjects[i]->id;
					//Set customAttribute interactable - should probably be removed since player isn't interactable
					if ((int)meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->customMayaAttribute->meshType == 1)
						gameObject_ptr->isInteractable = true;
					//Add BBox from leapmesh to player object
					for (int i = 0; i < meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->boundingBoxes.size(); i++)
					{
						bBox* box = new bBox();
						//add center
						box->center.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[0];
						box->center.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[1];
						box->center.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[2];
						//add max vector
						box->vMax.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[0];
						box->vMax.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[1];
						box->vMax.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[2];
						//add min vector
						box->vMin.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[0];
						box->vMin.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[1];
						box->vMin.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[2];
						//push into gameobject
						gameObject_ptr->bbox.push_back(box);
					}
					//break loop
					break;
				}
			}
		}
		else 
		{
			//Create new mesh object
			GameObject* meshObject = new GameObject();
			meshObject->name = "Mesh " + std::to_string(i);
			//Set mesh object position in world
			meshObject->transform->position = glm::vec3(level->levelObjects[i]->x, level->levelObjects[i]->y, level->levelObjects[i]->z);
			meshObject->transform->rotation = glm::vec3(level->levelObjects[i]->rotationX, level->levelObjects[i]->rotationY, level->levelObjects[i]->rotationZ);
			//Calculate new world Y-position from height map and update value
			float newPositionY = terrain->calculateY(meshObject->transform->position.x, meshObject->transform->position.z) - 2;
			meshObject->transform->position.y = newPositionY;
			//Add new mesh component with data from mesh library
			MeshFilter* meshFilter = new MeshFilter(
				meshLibrary.getMesh(level->levelObjects[i]->id)->gVertexBuffer,
				meshLibrary.getMesh(level->levelObjects[i]->id)->gVertexAttribute,
				meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->getVertexCount(),
				meshLibrary.getMesh(level->levelObjects[i]->id)->meshType);
			meshObject->addComponent(meshFilter);
			//Add material to gameObject from materialLibrary
			meshObject->addComponent(materialLibrary.getMaterial(0));
			//Set customAttribute ID from Enum.H
			meshObject->objectID = (ObjectType::ID)level->levelObjects[i]->id;
			//Set customAttribute interactable
			if ((int)meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->customMayaAttribute->meshType == 1)
				meshObject->isInteractable = true;
			//Add BBox from leapmesh to gameObject
			for (int i = 0; i < meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->boundingBoxes.size(); i++)
			{
				bBox* box = new bBox();
				//add center
				box->center.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[0];
				box->center.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[1];
				box->center.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[2];
				//add max vector
				box->vMax.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[0];
				box->vMax.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[1];
				box->vMax.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[2];
				//add min vector
				box->vMin.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[0];
				box->vMin.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[1];
				box->vMin.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[2];
				//push into gameobject
				meshObject->bbox.push_back(box);
			}
			//Add to scene
			gameObjects.push_back(meshObject);
		}
	}
}

void GameScene::addTerrain(const std::string & heightMap, GLuint shader)
{
	//Create terrain object
	GameObject* terrainObject = new GameObject();
	terrainObject->name = "Terrain";
	//Add terrain component
	Terrain * terrain = new Terrain(heightMap, shader);
	terrainObject->addComponent(terrain);
	//Add to scene
	gameObjects.push_back(terrainObject);
}

void GameScene::addMainMenu()
{
	GameObject* MainMenuObject = new GameObject();
	MainMenuScene* mainMenuScene = new MainMenuScene();
	MainMenuObject->name = "MainMenu";
	MainMenuObject->addComponent(mainMenuScene);
	MainMenuObject->setIsRenderable(true);
	
	gameObjects.push_back(MainMenuObject);
}


void GameScene::interactionTest(GameObject & other, GLFWwindow * window)
{
	for (GameObject* gameObject_ptr : gameObjects)
	{
		if (gameObject_ptr->getPlayer() != nullptr)
		{
			if (gameObject_ptr->getPlayer()->click == false && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS))
			{
				float distance = glm::distance(other.transform->position, gameObject_ptr->transform->position);
				if (distance < 10 && other.isInteractable == true)
				{
					gameObject_ptr->getPlayer()->click = true;
					RayData ray = Ray::getWorldRay(
						SCREEN_WIDTH*0.5f, SCREEN_HEIGHT*0.5f,
						gameObject_ptr->getViewMatrix(), 
						SCREEN_WIDTH, SCREEN_HEIGHT,
						gameObject_ptr->transform->position);

					for (int i = 0; i < other.bbox.size(); i++)
					{
						if (Intersection::rayBoxTest(ray, *other.bbox[i], other.getModelMatrix()))
						{
							std::cout << "HIT::" << other.name << std::endl;
							/*
							gameObject_ptr.doSomething(other.objectID); //use function inside the player class and make things happen.
							*/
							other.setIsRenderable(false);
							if (other.objectID == ObjectType::ID::BUCKET)
							{
								gameObject_ptr->getPlayer()->equip("BucketIcon");
								gameObject_ptr->getPlayer()->addImageToInventory("InventoryBucketIcon", 4);
							}
						}
						else
						{
							std::cout << "MISS::" << other.name << std::endl;
							//other.setIsRenderable(true);
						}
					}
				}
			}
			if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) && gameObject_ptr->getPlayer()->click == true)
				gameObject_ptr->getPlayer()->click = false;
		}
	}
}

void GameScene::collisionTest(GameObject & other)
{
	for (GameObject* gameObject_ptr : gameObjects)
	{
		if (gameObject_ptr->getPlayer() != nullptr && other.objectID != ObjectType::ID::PLAYER)
		{
			float distance = glm::distance(other.transform->position, gameObject_ptr->transform->position);
			if (distance < 10)
			{
				for (int i = 0; i < gameObject_ptr->bbox.size(); i++)
				{
					for (int j = 0; j < other.bbox.size(); j++)
					{
						if (Intersection::collisionTest(*gameObject_ptr->bbox[i], gameObject_ptr->transform->position, *other.bbox[j], other.transform->position))
						{
							std::cout << "GAMESCENE::collisionTest()::" << gameObject_ptr->name << " -> " << other.name << std::endl;
						}
					}
				}
			}
		}
	}
}

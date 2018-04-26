#include "GameScene.h"

GameScene::GameScene() :
	camerasInScene(0), lightsInScene(0)
{
	this->playerFound = true;
}

GameScene::~GameScene()
{
}

void GameScene::addEmptyGameObject()
{
	GameObject newEmptyGameObject = GameObject();
	gameObjects.push_back(newEmptyGameObject);
}

void GameScene::clearGameObjects()
{
	while (!gameObjects.empty())
	{
		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			for (int j = 0; i < gameObjects[i].components.size(); i++)
				gameObjects[i].deleteComponent(gameObjects[i].components[j]);
		}
		gameObjects.pop_back();
	}
	gameObjects.clear();
}

void GameScene::addLight(glm::vec3 transform, int lightType)
{
	lightsInScene++;
	addEmptyGameObject();
	Light* light = new Light(*gameObjects[gameObjects.size() - 1].transform);
	light->lightType = lightType;
	gameObjects[gameObjects.size() - 1].addComponent(light);
	gameObjects[gameObjects.size() - 1].name = "Light " + std::to_string(lightsInScene);
	gameObjects[gameObjects.size() - 1].transform->position = transform;
	//...
	//gameObjects[gameObjects.size() - 1].lightComponent->lightType = lightType; 
}

void GameScene::addPlayer()
{
	camerasInScene++;
	addEmptyGameObject();
	std::cout << "PLAYER INDEX:: " << gameObjects.size() - 1 << std::endl;
	Player* moveScript = new Player(*gameObjects[gameObjects.size()-1].transform);
	gameObjects[gameObjects.size()-1].name = "Player " + camerasInScene;
	gameObjects[gameObjects.size()-1].addComponent(moveScript);
	gameObjects[gameObjects.size() - 1].transform->position = glm::vec3(0.f, 0.f, 0.f);
	}

void GameScene::addMeshFilter(MeshLib & meshLibrary, MaterialLib& matertialLibrary, GLuint meshNameSize)
{
	/*
	meshNameSize should be replaced or removed
	*/

	/*
	This function should get data from the level file and create as many objects of each type that is needed
	to build the scene.
	*/

	/*
	We need this for each object in the scene:
		* World position
		* Mesh ID or mesh name to decide which mesh from the MeshLib to use when adding a new object to the scene.
		* ...
	*/

	for (int i = 0; i < meshNameSize; i++) // 3 - meshLibrary.getNumberOfMeshes(); meshName.size();
	{
		addEmptyGameObject();
		MeshFilter* meshFilter = new MeshFilter(meshLibrary.getMesh(i).gVertexBuffer, meshLibrary.getMesh(i).gVertexAttribute, meshLibrary.getMesh(i).leapMesh->getVertexCount(), meshLibrary.getMesh(i).meshType);
		gameObjects[gameObjects.size() - 1].name = "Mesh " + std::to_string(i); // Maybe pass the name of the object?
		gameObjects[gameObjects.size() - 1].addComponent(meshFilter);
		gameObjects[gameObjects.size() - 1].addComponent(matertialLibrary.getMaterial(0));
		//... set interactable
		//if(meshLibrary.getMesh(i).leapMesh->customMayaAttribute->meshType == 1)
		gameObjects[gameObjects.size() - 1].isInteractable = true; // if meshType 1, set true
		//std::cout << "MESHFILTER::" << gameObjects.size() - 1 << std::endl;

		//Add BBox from leapmesh to gameObject
		for (int i = 0; i < meshLibrary.getMesh(i).leapMesh->boundingBoxes.size(); i++)
		{
			//meshLibrary.getMesh(i).leapMesh->boundingBoxes
			bBox box = bBox();
			//add center
			box.center.x = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->center[0];
			box.center.y = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->center[0];
			box.center.z = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->center[0];
			//add max vector
			box.vMax.x = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->maxVector[0];
			box.vMax.y = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->maxVector[0];
			box.vMax.z = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->maxVector[0];
			//add min vector
			box.vMin.x = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->minVector[0];
			box.vMin.y = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->minVector[0];
			box.vMin.z = meshLibrary.getMesh(i).leapMesh->boundingBoxes[i]->minVector[0];
			//push into gameobject
			gameObjects[gameObjects.size() - 1].bbox.push_back(box);
		}
		if (gameObjects.size() - 1 == 4)
		{
			gameObjects[gameObjects.size() - 1].transform->position = glm::vec3(10,0,0);
		}
	}
}

void GameScene::addTerrain(const std::string & heightMap, GLuint shader)
{
	addEmptyGameObject();
	std::cout << "TERRAIN INDEX:: " << gameObjects.size() - 1 << std::endl;
	newTerrain = new Terrain(heightMap, shader);
	gameObjects[gameObjects.size() - 1].name = "Terrain";
	gameObjects[gameObjects.size() - 1].addComponent(newTerrain);
}

void GameScene::addMainMenu()
{
	addEmptyGameObject();
	MainMenuScene* mainMenuScene = new MainMenuScene();
	gameObjects[gameObjects.size() - 1].addComponent(mainMenuScene);
	gameObjects[gameObjects.size() - 1].setIsRenderable(true);
	// skräp
	addEmptyGameObject();
}

void GameScene::update(float deltaTime, float seconds)
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{

		if (gameObjects[i].getPlayer() != nullptr)
		{
			for (int j = 0; j < gameObjects.size(); j++)
			{
				glm::vec2 UVS = gameObjects[i].getPlayer()->setXZ();
				float u = UVS.x;
				float v = UVS.y;
				if (gameObjects[j].getTerrain() != nullptr)
				{
					gameObjects[i].getPlayer()->setCurrentHeight(gameObjects[j].getTerrain()->calculateY(u, v));
				}

			}

		}
		gameObjects[i].update(deltaTime, seconds);
	}

}

void GameScene::processEvents(GLFWwindow * window, float deltaTime)
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i].processEvents(window, deltaTime);
	}
	if (gameObjects[0].getPlayer() != nullptr)
	{
		if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && gameObjects[0].getPlayer()->click == false)
		{
			gameObjects[0].getPlayer()->click = true;
			// loop rayBoxIntersection here
			float x = 1280.0; //ScreenWidth
			float y = 720.0; //ScreenHeight
			RayData ray = Ray::getWorldRay(x*0.5f, y*0.5f, gameObjects[0].getViewMatrix(), x, y, gameObjects[0].transform->position);

			// crash when using for(int i=1; i < gameObjects.size(); i++)
			// vector subscript out of range
			// the last spot in the vector is the reason for the crash..

			//for (int i = 3; i < 4; i++)
			//for (int i = 1; i < gameObjects.size(); ++i)
			std::cout << "GameObjects.size() :: " << gameObjects.size() << std::endl;
			for (int i = 3; i < 5; i++)
			{
				//int i = 3;
				if (gameObjects[i].isInteractable)
				{
					for (int j = 0; j < gameObjects[i].bbox.size(); j++)
					{
						if (Intersection::rayBoxTest(ray, gameObjects[i].bbox[j], gameObjects[i].getModelMatrix()))
						{
							std::cout << "HIT::" << gameObjects[i].name << std::endl;
							gameObjects[i].setIsRenderable(false);
							//gameObjects[i].interactUpdate() ?
						}
						else
						{
							std::cout << "MISS" << std::endl;
							gameObjects[i].setIsRenderable(true);
						}
					}
				}
			}

			if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) && gameObjects[0].getPlayer()->click == true) {
				gameObjects[0].getPlayer()->click = false;
			}
		}
	}
}

Terrain* GameScene::getTerrainPointer()
{
	return this->newTerrain;
}

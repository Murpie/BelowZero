#include "GameScene.h"

GameScene::GameScene() :
	camerasInScene(0), lightsInScene(0)
{

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
	Player* moveScript = new Player(*gameObjects[gameObjects.size() - 1].transform);
	gameObjects[gameObjects.size() - 1].name = "Player " + std::to_string(camerasInScene);
	gameObjects[gameObjects.size() - 1].addComponent(moveScript);
}

void GameScene::addMeshFilter(MeshLib & meshLibrary, MaterialLib& matertialLibrary, GLuint meshNameSize)
{
	/*
	This function should get data from the level file and create as many objects of each type that is needed
	to build the scene.
	*/
	for (int i = 0; i < meshNameSize; i++) // 3 - meshLibrary.getNumberOfMeshes(); meshName.size();
	{
		addEmptyGameObject();
		MeshFilter* meshFilter = new MeshFilter(meshLibrary.getMesh(i).gVertexBuffer, meshLibrary.getMesh(i).gVertexAttribute, meshLibrary.getMesh(i).vertexCount, meshLibrary.getMesh(i).meshType);
		gameObjects[gameObjects.size() - 1].name = "Mesh " + std::to_string(i); // Maybe pass the name of the object?
		gameObjects[gameObjects.size() - 1].addComponent(meshFilter);
		gameObjects[gameObjects.size() - 1].addComponent(matertialLibrary.getMaterial(0));
	}
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
}

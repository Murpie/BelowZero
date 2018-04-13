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
	while(!gameObjects.empty())
	{
		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			for(int j = 0; i < gameObjects[i].components.size(); i++)
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
	addEmptyGameObject();
	gameObjects[gameObjects.size() - 1].addComponent(light);
	gameObjects[gameObjects.size() - 1].name = "Light " + lightsInScene;
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
}

void GameScene::addMeshFilter(MeshLib & meshLibrary, MaterialLib& matertialLibrary, int meshNameSize)
{
	/*
	This function should get data from the level file and create as many objects of each type that is needed
	to build the scene. 
	*/
	for (int i = 0; i < meshNameSize; i++) // 3 - meshLibrary.getNumberOfMeshes(); meshName.size();
	{
		addEmptyGameObject();
		MeshFilter* meshFilter = new MeshFilter(meshLibrary.getMesh(i).gVertexBuffer, meshLibrary.getMesh(i).gVertexAttribute, meshLibrary.getMesh(i).gElementBuffer, meshLibrary.getMesh(i).vertexCount);
		gameObjects[gameObjects.size() - 1].name = "Mesh " + i; // Maybe pass the name of the object?
		gameObjects[gameObjects.size() - 1].addComponent(meshFilter);
		gameObjects[gameObjects.size() - 1].addComponent(matertialLibrary.getMaterial(i));
	}
}

void GameScene::update(float deltaTime)
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i].update(deltaTime);
	}

}

void GameScene::processEvents(GLFWwindow * window, float deltaTime)
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i].processEvents(window, deltaTime);
	}
}

#include "GameScene.h"

GameScene::GameScene(Scene::ID sceneID)
{
	this->sceneID = sceneID;
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
		//for (unsigned int i = 0; i < gameObjects.size(); i++)
		//{
		//	for(int j = 0; i < gameObjects[i].components.size(); i++)
		//		gameObjects[i].deleteComponent(gameObjects[i].components[j]);
		//}
		gameObjects.pop_back();
	}
}

void GameScene::addLight()
{
	addEmptyGameObject();
	Light light = Light();
	lights.push_back(light);
}

void GameScene::addCharacterMovement(GLFWwindow * window)
{
	addEmptyGameObject();
	CharacterMovement tempMoveScript = CharacterMovement(window);
	moveScript.push_back(tempMoveScript);
}

void GameScene::update(float deltaTime)
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i].update(deltaTime);
	}

}
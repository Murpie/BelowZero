#pragma once
#include <vector>

#include "EnumID.h"
#include "GameObject.h"
#include "CharacterMovement.h"

class GameScene
{
public:
	GameScene(Scene::ID sceneID);
	~GameScene();

	std::vector<GameObject> gameObjects;
	std::vector<Light> lights;
	std::vector<CharacterMovement> moveScript;

	void addEmptyGameObject();
	void clearGameObjects();

	void addLight();
	void addCharacterMovement(GLFWwindow* window);

	void update(float deltaTime);

	Scene::ID sceneID;

private:

};
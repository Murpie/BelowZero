#pragma once
#include <vector>

#include "EnumID.h"
#include "GameObject.h"
#include "CharacterMovement.h"
#include "MeshLib.h"
#include "MaterialLib.h"

class GameScene
{
public:
	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = default;
	GameScene();
	~GameScene();

	// change these to pointers and delete them when we want to create a new scene. 
	// ... or just use the add component function?
	std::vector<GameObject> gameObjects; 

	void addEmptyGameObject();
	void clearGameObjects(); // delete all pointers in each gameobject and reload on scene swap

	void addLight(glm::vec3 transform, int lightType);
	void addCharacterMovement(GLFWwindow* window); //player
	void addMeshFilter(MeshLib& meshLibrary, MaterialLib& matertialLibrary, int meshNameSize);

	void update(float deltaTime);

private:

	int camerasInScene; //player
	int lightsInScene;

};
#pragma once
#include <vector>

#include "EnumID.h"
#include "GameObject.h"
#include "MeshLib.h"
#include "MaterialLib.h"

class GameScene
{
public:
	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = default;
	GameScene();
	~GameScene();

	std::vector<GameObject> gameObjects; 

	void addEmptyGameObject();
	// clearGameObjects is unused atm. 
	void clearGameObjects(); // delete all pointers in each gameobject and reload on scene swap

	void addLight(glm::vec3 transform, int lightType);
	void addPlayer(); //player
	void addMeshFilter(MeshLib& meshLibrary, MaterialLib& matertialLibrary, int meshNameSize);

	void update(float deltaTime); // Updates all the objects in the scene
	void processEvents(GLFWwindow *window, float deltaTime); // Updates objects if a evenet occurs, mouse press, button press etc. 

private:

	//...
	int camerasInScene; //player atm
	int lightsInScene; //

};
#pragma once
#include <vector>

#include "EnumID.h"
#include "GameObject.h"
#include "MeshLib.h"
#include "MaterialLib.h"
#include "ShaderProgramLib.h"

class GameScene
{
public:
	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = default;
	GameScene();
	~GameScene();

	std::vector<GameObject*> gameObjects; 

	void clearGameObjects();
	void addLight(glm::vec3 transform, int lightType);
	void addPlayer(); 
	void addLevelObjects(MeshLib& meshLibrary, MaterialLib& matertialLibrary, LeapLevel* level);
	void addTerrain(const std::string & heightMap, GLuint shader);

	void update(float deltaTime, float seconds); // Updates all the objects in the scene
	void processEvents(GLFWwindow *window, float deltaTime); // Updates objects if a evenet occurs, mouse press, button press etc. 
	void interactionTest(GameObject & other, GLFWwindow * window); // Intersection test on mouse click

	void addLevelScene(MeshLib& meshLibrary, MaterialLib& matertialLibrary, ShaderProgramLib& shader);

private:
	int camerasInScene; //player 
	int lightsInScene; 
};
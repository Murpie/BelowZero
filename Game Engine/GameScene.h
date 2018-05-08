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
	Scene::ID typeOfScene;

	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = default;
	GameScene(Scene::ID typeOfScene);
	~GameScene();

	std::vector<GameObject*> gameObjects; 

	void clearGameObjects();
	void update(float deltaTime, float seconds); // Updates all the objects in the scene
	void processEvents(GLFWwindow *window, float deltaTime); // Updates objects if a evenet occurs, mouse press, button press etc. 

	void initScene(MeshLib* meshLibrary, MaterialLib* matertialLibrary, ShaderProgramLib& shader, Scene::ID typeOfScene);

private:
	int camerasInScene; //player 
	int lightsInScene; 

	void addLight(glm::vec3 transform, int lightType);
	void addPlayer();
	void addLevelObjects(MeshLib& meshLibrary, MaterialLib& matertialLibrary, LeapLevel* level);
	void addTerrain(const std::string & heightMap, GLuint shader);
	void addMainMenu();

	void interactionTest(GameObject & other, GLFWwindow * window); // Intersection test on mouse click
	void collisionTest(GameObject & other); // Collision test when two AABB overlap

	void makeObjectsInteractable();

	void addGameObject(const glm::vec3 position);
	bool addObject;
	MaterialLib* material;
	MeshLib* meshes;
};
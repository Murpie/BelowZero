#pragma once
#include <vector>
#include <thread>

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

	void addAI(MeshLib& meshLibrary, MaterialLib& materialLibrary);
	void addLight(glm::vec3 transform, int lightType);
	void addPlayer(MeshLib& meshLibrary, MaterialLib& materialLibrary);
	void addEquipment();
	void addLevelObjects(MeshLib& meshLibrary, MaterialLib& matertialLibrary, LeapLevel* level);
	void addTerrain(const std::string & heightMap, GLuint shader);
	void addMainMenu();
	void checkInteractionResponse(GameObject & other, int objectID);

	void interactionTest(GameObject & other, GLFWwindow * window); // Intersection test on mouse click
	void collisionTest(GameObject & other, float deltaTime); // Collision test when two AABB overlap
	void aiCollisionTest(GameObject & other);

	void makeObjectsInteractable();

	void addGameObject(const glm::vec3 position, const int key);
	void addNewObjectTest(GLFWwindow * window);
	bool addObject;

	MaterialLib* material;
	MeshLib* meshes;

	//test functions
	void setBurningByDistance(const float distance, GameObject & other);
	void meltIceWall(GameObject & other);
	//
};
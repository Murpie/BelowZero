#pragma once
#include <vector>
#include "Component.h"
#include <string>
#include "MeshFilter.h"
#include "Material.h"
#include "Light.h"
#include <Windows.h>
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"
#include "Player.h"
#include "Terrain.h"
#include "MainMenuScene.h"

#include "Ray.h"
#include "Intersection.h"
#include <list>

class Component;

class GameObject 
{
public:
	GameObject();
	~GameObject();

	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);

    bool isActive;
    bool hasLight;
	bool isInteractable;
	bool gameEnd;
	bool moveBelowTerrain;
	float timeAlive;
	
	int playerHitCounter; //used to count how many hits the object has taken from player
	
	ObjectType::ID objectID;
	std::string name;
	Transform *transform;
	std::list<Component*> components;

	std::vector <bBox*> bbox;

    Material *materialComponent;
    MeshFilter *meshFilterComponent; 
	Light *lightComponent;
	Light *fireComponent;

    void updateMaterialAndMeshFilterPointers();
    void updateHasLight();
	void addComponent(Component* otherComponent);
	void updateMeshFilter(int id);
	//void addComponent(Component* otherComponent); // Maybe use this one when everthing works as intended
	void deleteComponent(Component* otherComponent);
	void deleteAllComponents();

    const bool getIsRenderable();
	void setIsRenderable(bool isRenderable);
	void setIsBurning(float timeToBurn);
	void setGameEnd();

	const bool getIsBurning();
	const bool getGameEnd();
	
	Player* getPlayer();
	glm::mat4 getModelMatrix();
	glm::mat4 getViewMatrix();
	Terrain* getTerrain();
	MainMenuScene* getMenuScene();

	template <class T>
	T* getComponent() {
		for (Component* component_ptr : components)
		{
			if (dynamic_cast<T*>(component_ptr) != nullptr)
			{
				T *test = dynamic_cast<T*>(component_ptr);
				return test;
			}
		}
		return nullptr;
	}

private:
    bool isRenderable;
	bool isBurning;
	float timeToBurn;

	glm::mat4 modelMatrix;

	void moveDown(float deltaTime);
	float timeLimit;
};
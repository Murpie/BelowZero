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

#include "Ray.h"
#include "Intersection.h"

class Component;

class GameObject //: public Component 
{
public:
	GameObject();
	~GameObject();

	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);

    bool isActive;
    bool hasLight;
	std::string name;
	Transform *transform;
	std::vector<Component*> components;

	/*
		rework functions so we can put Material, MeshFilter and Light
		directly into the component vector.

		We need to this to be able to effectivly delete the components
		in the components vector. This will be used every time we change scene
		so we don't have to store the data in memory all the time. 

		We will refill the component vector when we load a scene. 
	*/

    Material *materialComponent; // we want this one in components
    MeshFilter *meshFilterComponent; // we want this one in components
	Light *lightComponent; // we want this one in components

	/*
		Material, MeshFilter and Light are components. 
		Why are they not stored in componets?
	*/

    void updateMaterialAndMeshFilterPointers();
    void updateHasLight();
	void addComponent(Component* otherComponent);
	//void addComponent(Component* otherComponent); // Maybe use this one when everthing works as intended
	void deleteComponent(Component* otherComponent);
	void deleteAllComponents();

    const bool getIsRenderable();

	Player* getPlayer();
	Terrain* getTerrain();

	template <class T>
	T* getComponent() {
		for (int i = 0; i < components.size(); i++)
		{
			if (dynamic_cast<T*>(components[i]) != nullptr)
			{
				T *test = dynamic_cast<T*>(components[i]);
				return test;
			}
		}

		return nullptr;
	}
private:
    bool isRenderable;
};

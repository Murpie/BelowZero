#pragma once
#include <vector>
#include "Component.h"
#include <string>
#include "Transform.h"
#include "MeshFilter.h"
#include "Material.h"
#include "Light.h"
#include <Windows.h>
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"

class Component;

class GameObject //: public Component 
{
public:
	GameObject();
	~GameObject();

	void update(float deltaTime);

    bool isActive;
    bool hasLight;
	std::string name;
	Transform transform;
	std::vector<Component*> components;
    Material *materialComponent;
    MeshFilter *meshFilterComponent;
	Light *lightComponent;

	/*
		Naterial, MeshFilter and Light are components. 
		Why are they not stored in componets?
	*/

    void updateMaterialAndMeshFilterPointers();
    void updateHasLight();
	void addComponent(Component* otherComponent);
	//void addComponent(Component* otherComponent); // Maybe use this one when everthing works as intended
	void deleteComponent(Component* otherComponent);
	void deleteAllComponents();

    const bool getIsRenderable();

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

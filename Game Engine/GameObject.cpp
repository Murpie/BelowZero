#include "GameObject.h"

GameObject::GameObject()
{
	name = "EmptyGameObject";
	transform=Transform();
    isActive = true;
    isRenderable = false;
    hasLight = false;

}

GameObject::~GameObject()
{
	//deleteAllComponents();

	//These will probably give memory leaks if not deleted.

	//delete materialComponent;
	//delete meshFilterComponent;
	//delete lightComponent;
}

void GameObject::updateMaterialAndMeshFilterPointers() {
    bool matTest = false;
    bool meshTest = false;

    for (int i = 0; i < components.size(); i++) {
        Material* temp = getComponent<Material>();
        if (temp != nullptr) {
            materialComponent = temp;
            matTest = true;

        }
    }

    for (int i = 0; i < components.size(); i++) {
        MeshFilter* temp = getComponent<MeshFilter>();
        if (temp != nullptr) {
            meshFilterComponent = temp;
            meshTest = true;

        }
    }

    if (meshTest==true && matTest==true) {
        isRenderable = true;
    } else {
        isRenderable = false;
    }
}

void GameObject::updateHasLight() {
    bool foundLight = false;

    for (int i = 0; i < components.size(); i++) {
       
        Light* temp = getComponent<Light>();
        if (temp != nullptr) {
            foundLight = true;

			lightComponent = temp;

        }
    }

   
    if (foundLight == true) {
        hasLight = true;
    } else {
        hasLight = false;
    }
}

void GameObject::addComponent(Component* otherComponent)
{
	//check if component exist
	int index = -1;
	for (int i = 0; i < components.size(); i++)
	{
		if (*otherComponent == *components[i]) {
			index = i;
		}
	}
	//add if not
	if (index == -1) {
        otherComponent->gameObject = this;
		components.push_back(otherComponent);
	}
    updateMaterialAndMeshFilterPointers();
    updateHasLight();
}

void GameObject::deleteComponent(Component* otherComponent)
{
	//find component
	int index = -1;
	for (int i = 0; i < components.size(); i++)
	{
		if (*otherComponent == *components[i]) {
			index = i;
		}
	}
	if (index != -1) {
		components.erase(components.begin() + index);
	}
    updateMaterialAndMeshFilterPointers();
}

void GameObject::deleteAllComponents()
{
	for (int i = 0; i < components.size(); i++)
	{
		delete components[i];
	}
	components.clear();
}

const bool GameObject::getIsRenderable() {
    return isRenderable;
}

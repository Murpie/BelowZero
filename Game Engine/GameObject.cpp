#include "GameObject.h"

GameObject::GameObject()
{
	transform = new Transform();
	name = "";
    isActive = true;
    isRenderable = false;
    hasLight = false;
	isInteractable = false;
	modelMatrix = glm::mat4();
	objectID = ObjectType::ID::Stone_1;
}

GameObject::~GameObject()
{
	deleteAllComponents();
	delete transform;

	for (bBox* bbox_ptr : bbox)
		delete bbox_ptr;
	bbox.clear();
}

void GameObject::update(float deltaTime, float seconds)
{
	for (Component* components_ptr : components)
	{
		Component& component = *components_ptr;
		component.update(deltaTime, seconds);
	}
}

void GameObject::processEvents(GLFWwindow * window, float deltaTime)
{
	for (Component* component_ptr : components)
	{
		component_ptr->processEvents(window, deltaTime);
	}
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
	components.push_back(otherComponent);
	updateMaterialAndMeshFilterPointers();
	updateHasLight();
	//check if component exist
	//int index = -1;
	//for (int i = 0; i < components.size(); i++)
	//{
	//	if (*otherComponent == *components[i]) {
	//		index = i;
	//	}
	//}
	////add if not
	//if (index == -1) {
 //       //otherComponent->gameObject = this;
	//	components.push_back(otherComponent);
	//}
 //   updateMaterialAndMeshFilterPointers();
 //   updateHasLight();
}

void GameObject::updateMeshFilter(int id)
{
	for (Component* component_ptr : components)
	{
		if (component_ptr->id == ComponentType::ID::MESHFILTER)
		{
			MeshFilter* meshFilter = static_cast<MeshFilter*>(component_ptr);
			if (meshFilter->typeID == id)
			{
				meshFilterComponent = meshFilter;
			}
		}
	}
}

/*
void GameObject::addComponent(Component* otherComponent)
{
	components.push_back(otherComponent);
}
*/

void GameObject::deleteComponent(Component* otherComponent)
{
	////find component
	//int index = -1;
	//for (int i = 0; i < components.size(); i++)
	//{
	//	if (*otherComponent == *components[i]) {
	//		index = i;
	//	}
	//}
	//if (index != -1) {
	//	//delete components[index];
	//	components.erase(components.begin() + index);
	//}
 //   updateMaterialAndMeshFilterPointers();
}

void GameObject::deleteAllComponents()
{
	for (Component* component_ptr : components)
	{
		if(component_ptr->id != ComponentType::ID::MATERIAL)
			delete component_ptr;
	}
	components.clear();
}

const bool GameObject::getIsRenderable() {
    return isRenderable;
}

void GameObject::setIsRenderable(bool isRenderable)
{
	this->isRenderable = isRenderable;
}

Player * GameObject::getPlayer()
{
	for (Component* component_ptr : components)
	{
		if (component_ptr->id == ComponentType::ID::PLAYER)
		{
			Player* player = static_cast<Player*>(component_ptr);
			return player;
		}
	}
}

MainMenuScene * GameObject::getMenuScene()
{
	for (Component* component_ptr : components)
	{
		if (component_ptr->id == ComponentType::ID::MAINMENU)
		{
			MainMenuScene* mainMenu = static_cast<MainMenuScene*>(component_ptr);
			return mainMenu;
		}
	}
	return nullptr;
}

glm::mat4 GameObject::getModelMatrix()
{
	modelMatrix = glm::translate(glm::mat4(1), transform->position);
	//... 
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), transform->rotation);
	return this->modelMatrix;
}

glm::mat4 GameObject::getViewMatrix()
{
	return glm::lookAt(transform->position, transform->position + transform->forward, transform->up);
}

Terrain * GameObject::getTerrain()
{
	for (Component* component_ptr : components)
	{
		if (component_ptr->id == ComponentType::ID::TERRAIN)
		{
			Terrain* terrain = static_cast<Terrain*>(component_ptr);
			return terrain;
		}
	}
}

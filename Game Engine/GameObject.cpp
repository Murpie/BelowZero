#include "GameObject.h"

GameObject::GameObject()
{
	transform = new Transform();
	name = "EmptyGameObject";
    isActive = true;
    isRenderable = false;
    hasLight = false;
	isInteractable = false;
	modelMatrix = glm::mat4();
	objectID = ObjectType::ID::BUCKET;
	//transform.forward = glm::vec3(1, 0, 0);
	//transform.up = glm::vec3(0, 1, 0);
	//transform.right = glm::vec3(0, 0, 1);
}

GameObject::~GameObject()
{
	//deleteAllComponents();

	//These will probably give memory leaks if not deleted.

	//delete materialComponent;
	//delete meshFilterComponent;
	//delete lightComponent;
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
		std::cout << component_ptr << std::endl;
		if(component_ptr->id != ComponentType::ID::MATERIAL)
			delete component_ptr;
	}
	components.clear();

	//delete transform;
	//delete materialComponent;
	//delete meshFilterComponent;
	//delete lightComponent;
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

	//for (int i = 0; i < components.size(); i++)
	//{

	//	if (components[i]->id == ComponentType::ID::PLAYER)
	//	{
	//		Player* player = static_cast<Player*>(components[i]);
	//		return player;
	//	}
	//}
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
	for (int i = 0; i < components.size(); i++)
	{

		if (components[i]->id == ComponentType::ID::TERRAIN)
		{
			Terrain* terrain = static_cast<Terrain*>(components[i]);
			return terrain;
		}
	}
	return nullptr;
}

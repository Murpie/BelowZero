#include "GameObject.h"

GameObject::GameObject()
{
	transform = new Transform();
	name = "";
    isActive = true;
    isRenderable = false;
    hasLight = false;
	isInteractable = false;
	isBurning = false;
	gameEnd = false;
	lighterEquipped = false;
	this->moveBelowTerrain = false;
	timeLimit = 0.f;
	timeAlive = 0.0f;
	timeToBurn = 10.f;
	modelMatrix = glm::mat4();
	objectID = ObjectType::ID::Stone_1;
	fireComponent = nullptr;
	lighterComponent = nullptr;
	playerHitCounter = 0;
}

GameObject::~GameObject()
{
	deleteAllComponents();
	
	delete transform;

	for (bBox* bbox_ptr : bbox)
		delete bbox_ptr;
	bbox.clear();

	if (fireComponent != nullptr)
	{
		delete fireComponent;
	}
	if (lighterComponent != nullptr)
	{
		delete lighterComponent;
	}
}

void GameObject::update(float deltaTime, float seconds)
{
	if (isBurning == true)
	{
		timeAlive += deltaTime;
		if (timeAlive >= timeToBurn)
		{
			isBurning = false;
			if (fireComponent != nullptr)
			{
				delete fireComponent;
				fireComponent = nullptr;
			}
		}
	}

	if (moveBelowTerrain)
		moveDown(deltaTime);

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

void GameObject::setIsBurning(float timeToBurn)
{
	this->timeToBurn = timeToBurn;
	timeAlive = 0.0f;

	if (fireComponent == nullptr)
	{
		fireComponent = new Light(*transform);
		fireComponent->lightType = 1;
		fireComponent->color = glm::vec4(0.9, 0.2, 0, .5);
		fireComponent->Linear = 25;
		fireComponent->Quadratic = 0.15;
		fireComponent->offset = 9;
		fireComponent->intensity = 0.9;
	}

	isBurning = true;
}

void GameObject::setGameEnd()
{
	gameEnd = true;
}

void GameObject::setLighterEquipped()
{
	if (lighterComponent == nullptr)
	{
		lighterComponent = new Light(*transform);
		lighterComponent->lightType = 1;
		lighterComponent->color = glm::vec4(0.9, 0.2, 0, .5);
		lighterComponent->Linear = 25;
		lighterComponent->Quadratic = 0.15;
		lighterComponent->offset = 9;
		lighterComponent->intensity = 0.1;
		lighterComponent->isLighter = true;
	}
}

const bool GameObject::getIsBurning()
{
	return this->isBurning;
}

void GameObject::resetLighterEquipped()
{
	if (!lighterEquipped)
	{
		if (lighterComponent != nullptr)
		{
			delete lighterComponent;
			lighterComponent = nullptr;
		}
	}
}

int GameObject::getEquippedItem()
{
	Player* tempPlayer = getPlayer();
	
	return tempPlayer->currentlyEquipedItem;
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

void GameObject::moveDown(float deltaTime)
{
	timeLimit += deltaTime;
	if (timeLimit > 20)
		isActive = false;

	transform->position.y -= 2 * deltaTime;
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

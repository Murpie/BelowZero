#include "GameScene.h"

GameScene::GameScene() :
	camerasInScene(0), lightsInScene(0)
{

}

GameScene::~GameScene()
{
}

void GameScene::addEmptyGameObject()
{
	GameObject newEmptyGameObject = GameObject();
	gameObjects.push_back(newEmptyGameObject);
}

void GameScene::clearGameObjects()
{
	if (!gameObjects.empty())
	{
		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i].deleteAllComponents();
			//for(int j = 0; i < gameObjects[i].components.size(); i++)
			//	gameObjects[i].deleteComponent(gameObjects[i].components[j]);
		}
		gameObjects.clear();
	}
}

void GameScene::addLight(glm::vec3 transform, int lightType)
{
	lightsInScene++;
	addEmptyGameObject();
	Light* light = new Light(*gameObjects[gameObjects.size() - 1].transform);
	light->lightType = lightType;
	addEmptyGameObject();
	gameObjects[gameObjects.size() - 1].addComponent(light);
	gameObjects[gameObjects.size() - 1].name = "Light " + lightsInScene;
	gameObjects[gameObjects.size() - 1].transform->position = transform;
	//...
	//gameObjects[gameObjects.size() - 1].lightComponent->lightType = lightType; 
}

void GameScene::addPlayer()
{
	camerasInScene++;
	addEmptyGameObject();
	std::cout << "PLAYER INDEX:: " << gameObjects.size() - 1 << std::endl;
	Player* moveScript = new Player(*gameObjects[gameObjects.size()-1].transform);
	gameObjects[gameObjects.size()-1].name = "Player " + camerasInScene;
	gameObjects[gameObjects.size()-1].addComponent(moveScript);
	gameObjects[gameObjects.size() - 1].transform->position = glm::vec3(0.f, 0.f, 0.f);
	}

void GameScene::addMeshFilter(MeshLib & meshLibrary, MaterialLib& matertialLibrary, int meshNameSize)
{
	/*
	This function should get data from the level file and create as many objects of each type that is needed
	to build the scene. 
	*/
	for (int i = 0; i < meshNameSize; i++) // 3 - meshLibrary.getNumberOfMeshes(); meshName.size();
	{
		addEmptyGameObject();
		MeshFilter* meshFilter = new MeshFilter(meshLibrary.getMesh(i).gVertexBuffer, meshLibrary.getMesh(i).gVertexAttribute, meshLibrary.getMesh(i).gElementBuffer, meshLibrary.getMesh(i).vertexCount);
		//Material*
		gameObjects[gameObjects.size() - 1].name = "Mesh " + i; // Maybe pass the name of the object?
		gameObjects[gameObjects.size() - 1].addComponent(meshFilter);
		gameObjects[gameObjects.size() - 1].addComponent(matertialLibrary.getMaterial(i));
		//... set interactable
		gameObjects[gameObjects.size() - 1].isInteractable = true;
		gameObjects[gameObjects.size() - 1].setIsRenderable(true);
	}
}

void GameScene::update(float deltaTime)
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i].update(deltaTime);
	}

}

void GameScene::processEvents(GLFWwindow * window, float deltaTime)
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i].processEvents(window, deltaTime);
	}

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && gameObjects[0].getPlayer()->click == false)
	{
		gameObjects[0].getPlayer()->click = true;
		// loop rayBoxIntersection here
		float x = 1280.0; //ScreenWidth
		float y = 720.0; //ScreenHeight
		RayData ray = Ray::getWorldRay(x*0.5f, y*0.5f, gameObjects[0].getViewMatrix(), x, y, gameObjects[0].transform->position);
		
		// crash when using for(int i=1; i < gameObjects.size(); i++)
		// vector subscript out of range
		// the last spot in the vector is the reason for the crash..

		//for (int i = 3; i < 4; i++)
		//for (int i = 1; i < gameObjects.size(); ++i)
		std::cout << "GameObjects.size() :: "<< gameObjects.size() << std::endl;
		for (int i = 3; i < 4; i++)
		{
			if (gameObjects[i].isInteractable)
			{
				if (Intersection::rayBoxTest(ray, gameObjects[i].bbox, gameObjects[i].getModelMatrix()))
				{
					std::cout << "HIT::" << gameObjects[i].name << std::endl;
					gameObjects[i].setIsRenderable(false);
					//gameObjects[i].interactUpdate() ?
				}
				else
				{
					std::cout << "MISS" << std::endl;
					gameObjects[i].setIsRenderable(true);
				}
			}
		}
	}
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) && gameObjects[0].getPlayer()->click == true)
	{
		gameObjects[0].getPlayer()->click = false;
	}
}

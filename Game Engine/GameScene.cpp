#include "GameScene.h"

GameScene::GameScene() :
	camerasInScene(0), lightsInScene(0)
{

}

GameScene::~GameScene()
{
	clearGameObjects();
}

void GameScene::addEmptyGameObject()
{
	GameObject* newEmptyGameObject = new GameObject();
	gameObjects.push_back(newEmptyGameObject);
}

void GameScene::clearGameObjects()
{
	for (GameObject* gameObject_ptr : gameObjects)
		delete gameObject_ptr;
	gameObjects.clear();
}

void GameScene::addLight(glm::vec3 transform, int lightType)
{
	lightsInScene++;
	addEmptyGameObject();
	Light* light = new Light(*gameObjects[gameObjects.size() - 1]->transform);
	light->lightType = lightType;
	gameObjects[gameObjects.size() - 1]->addComponent(light);
	gameObjects[gameObjects.size() - 1]->name = "Light " + std::to_string(lightsInScene);
	gameObjects[gameObjects.size() - 1]->transform->position = transform;
}

void GameScene::addPlayer()
{
	camerasInScene++;
	addEmptyGameObject();
	std::cout << "PLAYER INDEX:: " << gameObjects.size() - 1 << std::endl;
	Player* moveScript = new Player(*gameObjects[gameObjects.size()-1]->transform);
	gameObjects[gameObjects.size()-1]->name = "Player " + camerasInScene;
	gameObjects[gameObjects.size()-1]->addComponent(moveScript);
	gameObjects[gameObjects.size() - 1]->transform->position = glm::vec3(0.f, 0.f, 0.f);
}

void GameScene::addMeshFilter(MeshLib & meshLibrary, MaterialLib& matertialLibrary, LeapLevel* level)
{
	/*
	meshNameSize should be replaced or removed
	*/

	/*
	This function should get data from the level file and create as many objects of each type that is needed
	to build the scene.
	*/

	/*
	We need this for each object in the scene:
		* World position
		* Mesh ID or mesh name to decide which mesh from the MeshLib to use when adding a new object to the scene.
		* ...
	*/

	for (int i = 0; i < level->levelObjects.size(); i++) // 3 - meshLibrary.getNumberOfMeshes(); meshName.size();
	{
		addEmptyGameObject();

		gameObjects[gameObjects.size() - 1]->transform->position =
			glm::vec3(level->levelObjects[i]->x, level->levelObjects[i]->y, level->levelObjects[i]->z);
		
		MeshFilter* meshFilter = 
			new MeshFilter(meshLibrary.getMesh(level->levelObjects[i]->id)->gVertexBuffer, 
				meshLibrary.getMesh(level->levelObjects[i]->id)->gVertexAttribute,
				meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->getVertexCount(),
				meshLibrary.getMesh(level->levelObjects[i]->id)->meshType);
		
		float temp = gameObjects[1]->getTerrain()->calculateY(gameObjects[gameObjects.size() - 1]->transform->position.x, gameObjects[gameObjects.size() - 1]->transform->position.z) -2;

		/*meshFilter->worldTransform = glm::translate(meshFilter->worldTransform, glm::vec3(uv.x, temp, uv.y));*/
		gameObjects[gameObjects.size() - 1]->transform->position.y = temp;
		gameObjects[gameObjects.size() - 1]->name = "Mesh " + std::to_string(i); // Maybe pass the name of the object?
		gameObjects[gameObjects.size() - 1]->addComponent(meshFilter);
		gameObjects[gameObjects.size() - 1]->addComponent(matertialLibrary.getMaterial(0));
		gameObjects[gameObjects.size() - 1]->objectID = (ObjectType::ID)level->levelObjects[i]->id;

		//... set interactable
		if((int)meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->customMayaAttribute->meshType == 1)
			gameObjects[gameObjects.size() - 1]->isInteractable = true; // if meshType 1, set true

		//Add BBox from leapmesh to gameObject
		for (int i = 0; i < meshLibrary.getMesh(level->levelObjects[i]->id)->leapMesh->boundingBoxes.size(); i++)
		{
			//meshLibrary.getMesh(i).leapMesh->boundingBoxes
			bBox box = bBox();
			//add center
			box.center.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[0];
			box.center.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[0];
			box.center.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->center[0];
			//add max vector
			box.vMax.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[0];
			box.vMax.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[0];
			box.vMax.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->maxVector[0];
			//add min vector
			box.vMin.x = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[0];
			box.vMin.y = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[0];
			box.vMin.z = meshLibrary.getMesh(i)->leapMesh->boundingBoxes[i]->minVector[0];
			//push into gameobject
			gameObjects[gameObjects.size() - 1]->bbox.push_back(box);
		}
	}
}

void GameScene::addTerrain(const std::string & heightMap, GLuint shader)
{
	addEmptyGameObject();
	std::cout << "TERRAIN INDEX:: " << gameObjects.size() - 1 << std::endl;
	Terrain * newTerrain = new Terrain(heightMap, shader);
	gameObjects[gameObjects.size() - 1]->name = "Terrain";
	gameObjects[gameObjects.size() - 1]->addComponent(newTerrain);
}

void GameScene::update(float deltaTime, float seconds)
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{

		if (gameObjects[i]->getPlayer() != nullptr)
		{
			for (int j = 0; j < gameObjects.size(); j++)
			{
				glm::vec2 UVS = gameObjects[i]->getPlayer()->setXZ();
				float u = UVS.x;
				float v = UVS.y;
				if (gameObjects[j]->getTerrain() != nullptr)
				{

					gameObjects[i]->getPlayer()->setCurrentHeight(gameObjects[j]->getTerrain()->calculateY(u, v));
				}

			}

		}
		gameObjects[i]->update(deltaTime, seconds);
	}

}

void GameScene::processEvents(GLFWwindow * window, float deltaTime)
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->processEvents(window, deltaTime);
	}

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && gameObjects[0]->getPlayer()->click == false)
	{
		gameObjects[0]->getPlayer()->click = true;
		// loop rayBoxIntersection here
		float x = 1280.0; //ScreenWidth
		float y = 720.0; //ScreenHeight
		RayData ray = Ray::getWorldRay(x*0.5f, y*0.5f, gameObjects[0]->getViewMatrix(), x, y, gameObjects[0]->transform->position);

		std::cout << "GameObjects.size() :: "<< gameObjects.size() << std::endl;
		for (int i = 1; i <  gameObjects.size(); i++)
		{
			if (gameObjects[i]->isInteractable)
			{
				for (int j = 0; j < gameObjects[i]->bbox.size(); j++)
				{
					if (Intersection::rayBoxTest(ray, gameObjects[i]->bbox[j], gameObjects[i]->getModelMatrix()))
					{
						std::cout << "HIT::" << gameObjects[i]->name << std::endl;
						gameObjects[i]->setIsRenderable(false);
						//gameObjects[i].interactUpdate() ?
						if (gameObjects[i]->objectID == ObjectType::ID::BUCKET)
						{
							gameObjects[0]->getPlayer()->equip("BucketIcon");
							gameObjects[0]->getPlayer()->addImageToInventory("InventoryBucketIcon", 4);
						}
					}
					else
					{
						std::cout << "MISS" << std::endl;
						gameObjects[i]->setIsRenderable(true);
					}
				}
			}
		}
	}
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) && gameObjects[0]->getPlayer()->click == true)
	{
		gameObjects[0]->getPlayer()->click = false;
	}

	//....
	//for (int i = 1; i < gameObjects.size(); i++)
	//{
	//	for (int j = 0; j < gameObjects[i].bbox.size(); j++)
	//	{
	//		float normalX, normalY, normalZ;
	//		float collisiontime = Intersection::sweptAABB(gameObjects[0].bbox[0], gameObjects[i].bbox[j], normalX, normalY, normalZ, gameObjects[0].transform);
	//		gameObjects[0].bbox[0].center.x += gameObjects[0].transform->velocity.x * collisiontime;
	//		gameObjects[0].bbox[0].center.y += gameObjects[0].transform->velocity.y * collisiontime;
	//		gameObjects[0].bbox[0].center.z += gameObjects[0].transform->velocity.z * collisiontime;
	//		float remainingtime = 1.0f - collisiontime;
	//		// slide
	//		/* add solution for the z-axis*/
	//		float dotprod = (gameObjects[0].transform->velocity.x * normalY + gameObjects[0].transform->velocity.y * normalX) * remainingtime;
	//		gameObjects[0].transform->velocity.x = dotprod * normalY;
	//		gameObjects[0].transform->velocity.y = dotprod * normalX;
	//		//gameObjects[0].transform->velocity.z = dotprod * normalx;
	//	}
	//}

}

//Terrain* GameScene::getTerrainPointer()
//{
//	return this->newTerrain;
//}

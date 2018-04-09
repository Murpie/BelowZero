#pragma once
#include <vector>
#include "GameObject.h"

class GameScene
{
public:
	GameScene();
	~GameScene();

	std::vector<GameObject> gameObjects;
	void addEmptyGameObject();
	void update();


private:

};
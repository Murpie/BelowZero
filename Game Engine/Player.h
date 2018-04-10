#pragma once
#include<string>
#include <iostream>
#include "stb_image.h"
#include <GL/gl3w.h>

class Player
{
public:
	Player();
	~Player();

	float hp;
	float cold;
	float coldMeter;
	float water;
	float waterMeter;
	float food;
	float foodMeter;
	float coldTick;
	float waterTick;
	float foodTick;
	float damage;

	int equiped;
	int inventory[5];
	int inventoryCount;

	unsigned int equipedFBO;
	unsigned int equipedTexture;

	void updateStats(float deltaTime);
	void setCold(float value);
	void setWater(float value);
	void setFood(float value);

	void addToInventory(int item);
	void equip(int item);

private:
};

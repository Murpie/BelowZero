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

	int initializer;
	int inventory[5];
	int inventoryCount;

	std::string imageNames[5] = {"InventoryAxe", "InventoryLighter", "InventoryLog", "InventoryFood", "InventoryBucket"};

	unsigned int equipedFBO;
	unsigned int equipedTexture;
	unsigned int inventoryFBO[5];
	unsigned int inventoryTexture[5];

	void updateStats(float deltaTime);
	void setCold(float value);
	void setWater(float value);
	void setFood(float value);

	void initiateInventoryTextures(std::string item);
	void addToInventory(int item);
	void equip(std::string item);
	void addImageToInventory(std::string item, int inventorySlot);

private:
};

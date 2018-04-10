#pragma once
#include<string>

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

	void updateStats(float deltaTime);
	void setCold(float value);
	void setWater(float value);
	void setFood(float value);

	void addToInventory(int item);
	void equip(int item);

private:
};

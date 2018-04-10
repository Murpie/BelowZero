#include "Player.h"

Player::Player()
{
	this->hp = 50;
	this->cold = 100;
	this->coldMeter = 0;
	this->water = 100;
	this->waterMeter = 0;
	this->food = 100;
	this->foodMeter = 0;
	this->coldTick = 20;
	this->waterTick = 20;
	this->foodTick = 20;
	this->damage = 0;
	this->equiped = 0;
	for (int i = 0; i < 5; i++)
		this->inventory[i] = 0;
	this->inventoryCount = 0;
}

Player::~Player()
{
}

void Player::updateStats(float deltaTime)
{
	// LOOSING HP
	if (this->cold < 20)
		this->coldMeter = 4;

	if (this->water < 20)
		this->waterMeter = 4;

	if (this->food < 20)
		this->foodMeter = 4;

	// REGENERATION
	if (this->cold > 80 && this->water > 80 && this->food > 80)
	{
		this->coldMeter = -4;
		this->foodMeter = -4;
		this->waterMeter = -4;
	}
	// NEUTRAL
	else if ((this->cold < 80 || this->water < 80 || this->food < 80) && this->cold > 20 && this->water > 20 && this->food > 20)
	{
		this->coldMeter = 0;
		this->foodMeter = 0;
		this->waterMeter = 0;
	}

	// DECREASE OVER TIME
	if (this->cold <= 100)
		this->cold = this->cold - (this->coldTick * deltaTime);

	if (this->water <= 100)
		this->water = this->water - (this->waterTick * deltaTime);

	if (this->food <= 100)
		this->food = this->food - (this->foodTick * deltaTime);

	this->damage = this->coldMeter + this->waterMeter + this->foodMeter;

	// HP DMG / REG
	if (this->hp < 100 && this->hp > 0)
		this->hp = this->hp - (this->damage * deltaTime);
}

void Player::setCold(float value)
{
	this->cold += value;

	if (this->cold > 100)
		this->cold = 100;
}

void Player::setWater(float value)
{
	this->water += value;

	if (this->water > 100)
		this->water = 100;
}

void Player::setFood(float value)
{
	this->food += value;

	if (this->food > 100)
		this->food = 100;
}

void Player::addToInventory(int item)
{
	this->inventory[this->inventoryCount] = item;
	
	if (inventoryCount < 5)
		inventoryCount++;
}

void Player::equip(int item)
{
	this->equiped = item;
}

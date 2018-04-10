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

	// ----------========== Main Menu Scene FrameBuffer ==========----------
	int width, height, nrOfChannels;
	unsigned char * data = stbi_load("axeTexture.png", &width, &height, &nrOfChannels, 0);

	glGenFramebuffers(1, &equipedFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, equipedFBO);

	glGenTextures(1, &equipedTexture);
	glBindTexture(GL_TEXTURE_2D, equipedTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load MainMenu Texture from path" << std::endl;
	}

	stbi_image_free(data);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, equipedTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Main Menu Framebuffer not complete!" << std::endl;
}

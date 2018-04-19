#pragma once
#include<string>
#include <iostream>
#include "stb_image.h"
#include <GL/gl3w.h>
#include "Transformable.h"

class Player : public Transformable
{
public:
	Player(Transform& transform);
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



	int inventory[5];
	int inventoryCount;

	unsigned int equipedFBO;
	unsigned int equipedTexture;

	void setCold(float value);
	void setWater(float value);
	void setFood(float value);

	void addToInventory(int item);
	void equip(std::string item);

	void recieveTerrainInformation(float currentHeight, float frontV, float backV, float leftV, float rightV, float distance);

	//Physics
	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);
private:


	bool frontCollision = false;
	bool bottomCollision = false;
	bool leftCollision = false;
	bool rightCollision = false;
	bool backCollision = false;
	bool topCollision = false;

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float pitch;
	float yaw;
	bool firstMouse;
	float lastX, lastY;
	double xpos, ypos;
	int mouseDisable;
	float xoffset;
	float yoffset;
	float sensitivity;

	float time = 0.0;

	float jumpSpeed = 7.64;
	float cameraSpeed = 7.06;
	float fallSpeed = 9.82;
	//--------=====Jumping=====-----------

	bool jumpReady = true;
	bool inAir = false;
	float timeInAir = 0.3;
	bool gravity = false;

	//Terrain
	float currentY;
	float frontVertexHeight;
	float backVertexHeight;
	float leftVertexHeight;
	float rightVertexHeight;
	float distanceToNextVertex;
};

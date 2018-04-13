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
	float fade;

	bool startGame;

	int inventory[5];
	int inventoryCount;

	unsigned int equipedFBO;
	unsigned int equipedTexture;

	void setCold(float value);
	void setWater(float value);
	void setFood(float value);

	void addToInventory(int item);
	void equip(std::string item);

	//Physics
	void update(float deltaTime);
	void processEvents(GLFWwindow *window, float deltaTime);
private:

	bool frontCollision = false;
	bool bottomCollision = false;
	bool leftCollision = false;
	bool rightCollision = false;
	bool backCollision = false;
	bool topCollision = false;
	float cameraSpeed;
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

	float time;

	//--------=====Jumping=====-----------
	bool jump = false;
	bool inAir = false;
	float timeInAir = 1.1;
};

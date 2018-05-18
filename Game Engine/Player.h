#pragma once
#include<string>
#include <iostream>
#include "stb_image.h"
#include <GL/gl3w.h>
#include "Transformable.h"
#include "SoundMaster.h"

class Player : public Transformable
{
public:
	Player(Transform& transform);
	~Player();

	SoundMasterSFML SnowCrunch;
	SoundMasterSFML AmbientMusic;
	SoundMasterSFML AmbientWind;
	SoundMasterSFML Swing;
	SoundMasterSFML HitWAxe;

	bool click;

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
	float winFade;
	float textFade;
	float flareTimer;

	std::string equipedItem;

	bool startGame;
	bool textOnScreen;
	bool win;
	
	int currentlyEquipedItem;
	int initializer;
	int textInitializer;
	int inventory[5];
	int maxAmountOfItems;
	int inventoryCount;

	std::string imageNames[5] = { "InventoryAxeIcon", "InventoryLighterIcon", "InventoryLogIcon", "InventoryFoodIcon", "MainMenuConcept" };
	std::string imagesCurrentlyInInventory[5] = { "EmptyImage", "EmptyImage", "EmptyImage", "EmptyImage", "EmptyImage" };

	unsigned int equipedFBO;
	unsigned int equipedTexture;
	unsigned int inventoryFBO[5];
	unsigned int inventoryTexture[5];
	unsigned int textFBO;
	unsigned int textTexture;

	void setCold(float value);
	void setWater(float value);
	void setFood(float value);

	void initiateInventoryTextures(std::string item);
	void addToInventory(int item);
	void equip(std::string item);
	void addImageToInventory(std::string item, int inventorySlot);
	bool checkInventory(std::string item);
	void addTextToScreen(std::string item);
	void swappingItem(float deltaTime);

	void recieveTerrainInformation(float currentHeight, float frontV, float backV, float leftV, float rightV, float distance, int nrof);
	void setCurrentHeight(float height);
	void setIsWalkable(bool walkable);
	glm::vec2 setXZ();
	//Physics
	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);

	//glm::mat4 getViewMatrix()const;
	void swingTest();
	void eatFood();
	int interactionResponse(const ObjectType::ID id, bool & isAlive);
	int collisionResponse(const ObjectType::ID);
	void heatResponse();
	void takeDamange(float damage, float deltaTime);
	int getEquipedItem();
	const int getEquipedID();
	bool addClick;

	float oldPitch;
	float oldYaw;
	float pickUp;

private:
	bool isColliding = false;

	float foodTimer = 0.0;

	bool movingForward = false;
	bool movingBackwards = false;
	bool movingLeft = false;
	bool movingRight = false;

	bool inInventory[5];
	bool isPressed;

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	glm::vec3 lastPos;
	glm::vec3 lastPosTemp;

	int equipedID;
	bool swapItem;
	bool pullDown;
	int equipItem;

	bool firstMouse;
	float lastX, lastY;
	double xpos, ypos;
	int mouseDisable;
	float xoffset;
	float yoffset;
	float sensitivity;
	float pitch;
	float yaw;

	float time = 0.0;
	float textTimer = 0.0;

	float jumpSpeed = 7.64;
	float cameraSpeed = 7.06;
	float fallSpeed = 9.82;
	//--------=====Jumping=====-----------

	bool jumpReady = true;
	bool inAir = false;
	float timeInAir = 0.3;
	bool gravity = false;

	//------=====Walking=====----
	bool isWalking = false;

	//Terrain
	float previousY;
	float currentY;
	float frontVertexHeight;
	float backVertexHeight;
	float leftVertexHeight;
	float rightVertexHeight;
	float distanceToNextVertex;

	int vertexLength;

	void findY();
};

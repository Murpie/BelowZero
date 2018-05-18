#include "Player.h"

Player::Player(Transform& transform) : Transformable(transform)
{
	Component::id = ComponentType::ID::PLAYER;

	click = false;
	addClick = false;

	this->currentlyEquipedItem = -1;
	this->equipedID = -1;
	this->pickUp = -1;
	this->swapItem = false;
	this->pullDown = false;
	this->jacket = false;
	this->bucketContent = 0;
	this->swing = false;
	this->axeSwing = 0;

	this->hp = 80;
	this->cold = 100;
	this->coldMeter = 0;
	this->water = 100;
	this->waterMeter = 0;
	this->food = 100;
	this->foodMeter = 0;
	this->coldTick = 0.5;
	this->waterTick = 0.5;
	this->foodTick = 0.5;
	this->damage = 0;
	this->initializer = 0;
	this->textInitializer = 0;
	this->inventoryCount = 0;
	this->maxAmountOfItems = 5;
	this->fade = 0.9f;
	this->winFade = 0;
	this->flareTimer = 0;
	this->textFade = 1;
	this->startGame = true;
	this->isPressed = false;
	this->win = false;
	for (int i = 0; i < 5; i++)
		this->inventory[i] = 0;
	this->inventoryCount = 0;
	/**/
	assetName = "CharacterMovement";
	cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0;
	yaw = 0;
	oldPitch = 0;
	oldYaw = 0;
	firstMouse = true;
	lastX = 400;
	lastY = 300;
	xpos = 0;
	ypos = 0;
	mouseDisable = 0;
	xoffset = 0;
	yoffset = 0;
	sensitivity = 0.002f;
	
	for (int i = 0; i < sizeof(inInventory); i++)
	{
		inInventory[i] = false;
	}

	equip("EmptyImage");
	for (int i = 0; i < 5; i++)
		initiateInventoryTextures("EmptyImage");
	addTextToScreen("EmptyImageTexture");

	SnowCrunch.addSound("Snow.wav");
	SnowCrunch.setVolume(50.0f);

	AmbientWind.addSound("Wind.wav");
	AmbientWind.playSound();
	AmbientWind.loop(true);

	//AmbientMusic.addSound("AmbientMusic1.wav");
	//AmbientMusic.setVolume(50.0f);
	//AmbientMusic.playSound();
	//AmbientMusic.loop(true);

	Swing.addSound("woosh.wav");

	HitWAxe.addSound("AxeHit.ogg");
}

Player::~Player()
{
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

void Player::initiateInventoryTextures(std::string item)
{
	std::string texturePNG = "Texture.png";
	std::string filePath = item + texturePNG;
	int width, height, nrOfChannels;

	// ----------========== Equipment FrameBuffer ==========----------
	unsigned char * data = stbi_load(filePath.c_str(), &width, &height, &nrOfChannels, 0);

	glGenFramebuffers(1, &inventoryFBO[inventoryCount]);
	glBindFramebuffer(GL_FRAMEBUFFER, inventoryFBO[inventoryCount]);

	glGenTextures(1, &inventoryTexture[inventoryCount]);
	glBindTexture(GL_TEXTURE_2D, inventoryTexture[inventoryCount]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load Inventory Texture from path" << std::endl;
	}

	stbi_image_free(data);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inventoryTexture[inventoryCount], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Inventory Framebuffer not complete!" << std::endl;
	inventoryCount++;
	if (inventoryCount == 5)
		inventoryCount = 0;
}

void Player::addToInventory(int item)
{
	this->inventory[this->inventoryCount] = item;

	if (inventoryCount < 5)
		inventoryCount++;
}

void Player::equip(std::string item)
{
	std::string texturePNG = "Texture.png";
	std::string filePath = item + texturePNG;
	int width, height, nrOfChannels;

	// ----------========== Equipment FrameBuffer ==========----------
	unsigned char * data = stbi_load(filePath.c_str(), &width, &height, &nrOfChannels, 0);

	if (this->initializer == 0)
		glGenFramebuffers(1, &equipedFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, equipedFBO);

	if (this->initializer == 0)
	{
		glGenTextures(1, &equipedTexture);
		this->initializer = 1;
	}
	glBindTexture(GL_TEXTURE_2D, equipedTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (data)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else
		std::cout << "Failed to load Equiped Texture from path" << std::endl;

	stbi_image_free(data);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, equipedTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Equiped Framebuffer not complete!" << std::endl;

}

void Player::addImageToInventory(std::string item, int inventorySlot)
{
	if (checkInventory(item) && item != "EmptyImage")
	{
		if (this->textTimer >= 1.0f || this->textOnScreen == false)
		{
			std::cout << "Item already exists in players inventory" << std::endl;
			addTextToScreen("Text-ItemAlreadyEquipped");
		}
	}
	else
	{
		this->currentlyEquipedItem = inventorySlot;
		std::string texturePNG = "Texture.png";
		std::string filePath = item + texturePNG;
		int width, height, nrOfChannels;

		// ----------========== Equipment FrameBuffer ==========----------
		unsigned char * data = stbi_load(filePath.c_str(), &width, &height, &nrOfChannels, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, inventoryFBO[inventorySlot]);

		glBindTexture(GL_TEXTURE_2D, inventoryTexture[inventorySlot]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Failed to load Inventory Texture from path" << std::endl;
		}

		stbi_image_free(data);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inventoryTexture[inventorySlot], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Inventory Framebuffer not complete!" << std::endl;

		this->imagesCurrentlyInInventory[inventorySlot] = item;
		inventoryCount++;
	}
}

bool Player::checkInventory(std::string item)
{
	bool check = false; // Item does not already exist in inventory

	for (int i = 0; i < maxAmountOfItems && check == false; i++)
	{
		if (item.c_str() == imagesCurrentlyInInventory[i])
			check = true; // Item already exists in players inventory
	}

	return check;
}

void Player::addTextToScreen(std::string item)
{
	std::string texturePNG = ".png";
	std::string filePath = item + texturePNG;
	int width, height, nrOfChannels;

	// ----------========== Equipment FrameBuffer ==========----------
	unsigned char * data = stbi_load(filePath.c_str(), &width, &height, &nrOfChannels, 0);

	if (this->textInitializer == 0)
		glGenFramebuffers(1, &textFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, textFBO);

	if (this->textInitializer == 0)
	{
		glGenTextures(1, &textTexture);
		this->textInitializer = 1;
	}
	glBindTexture(GL_TEXTURE_2D, textTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load Text Texture from path" << std::endl;
	}

	stbi_image_free(data);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Text Framebuffer not complete!" << std::endl;

	if (item == "EmptyImageTexture")
		this->textOnScreen = false;
	else
	{
		this->textOnScreen = true;
		this->textFade = 1.0;
	}

	this->textTimer = 0.0;

}

void Player::swappingItem(float deltaTime)
{
	if (swapItem)
	{
		if (!pullDown)
		{
			if (pickUp >= 0)
				swapItem = false;
			else
				pickUp += deltaTime * 3;
		}
		else
		{
			if (pickUp <= -1)
			{
				pullDown = false;
				this->equipedID = equipItem;
			}
			else
				pickUp -= deltaTime * 4;
		}
	}
}

void Player::dropItem()
{
	pickUp = -1;
}

void Player::useItem(GLFWwindow * window)
{
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && this->currentlyEquipedItem == 0 && pickUp >= 0 && !swing)
	{
		swing = true;
		axeSwing = 0;
	}

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && this->currentlyEquipedItem == 4 && pickUp >= 0)
	{
		if (bucketContent == 0)
		{
			bucketContent = 1;
			equipItemMesh();
			swapItem = true;
			pullDown = true;
		}
	}
}

void Player::recieveTerrainInformation(float currentHeight, float frontV, float backV, float leftV, float rightV, float distance, int nrof)
{
	this->currentY = currentHeight;
	this->frontVertexHeight = frontV;
	this->backVertexHeight = backV;
	this->leftVertexHeight = leftV;
	this->rightVertexHeight = rightV;
	this->distanceToNextVertex = distance;
	this->vertexLength = nrof;

}

void Player::setCurrentHeight(float height)
{
	if (isColliding)
		previousY = currentY;

	currentY = height + 7;
}

void Player::setIsWalkable(bool walkable)
{
	if (walkable)
		isColliding = false;
	else
		isColliding = true;
}

glm::vec2 Player::setXZ()
{
	float u = Transformable::transform.position.x;
	float v = Transformable::transform.position.z;


	return glm::vec2(u, v);
}

void Player::update(float deltaTime, float seconds)
{
	//update velocity
	//Transformable::transform.velocity = Transformable::transform.forward * deltaTime;
	//...
	if (isColliding && lastPos.y < currentY)
	{
		Transformable::transform.position = lastPos;
		currentY = lastPos.y;
	}
	if (time <= timeInAir && inAir == true)
	{
		glm::vec3 jumpdir = Transformable::transform.up;

		Transformable::transform.position += jumpSpeed * jumpdir * deltaTime;
		Transformable::transform.velocity = Transformable::transform.up * deltaTime * cameraSpeed;
	}
	else
		inAir = false;


	if (inAir == false && Transformable::transform.position.y <= currentY)
	{
		gravity = false;
		jumpReady = true;
	}
	else
		gravity = true;


	if (gravity == true && inAir == false)
		Transformable::transform.position -= fallSpeed * Transformable::transform.up  * deltaTime;


	if (Transformable::transform.position.y <= currentY - 0.0001)
		Transformable::transform.position.y = currentY;

	if (isWalking == true)
	{
		if (!SnowCrunch.isPlaying())
			SnowCrunch.playSound();
	}
	else
	{
		SnowCrunch.stopSound();
	}
	//=======================-------------Leons Test Sak------------==================

	swappingItem(deltaTime);

	if (swing)
		swingAxe(deltaTime);

	if (win)
	{
		if (flareTimer >= 10.0f)
			stateOfGame.state = Gamestate::ID::CLEAR_LEVEL;
	}
	else if (fade >= 1.0f)
	{
		stateOfGame.state = Gamestate::ID::CLEAR_LEVEL;
	}

	float tempSeconds = seconds / 1000;
	time += tempSeconds;


	this->foodTimer += tempSeconds;

	this->textTimer += tempSeconds;
	if (this->textTimer >= 1.0f && this->textOnScreen == true)
	{
		addTextToScreen("EmptyImageTexture");
	}

	// LOOSING HP
	if (this->cold < 20)
		this->coldMeter = 0.5;

	if (this->water < 20)
		this->waterMeter = 0.5;

	if (this->food < 20)
		this->foodMeter = 0.5;

	// REGENERATION
	if (this->cold > 80 && this->water > 80 && this->food > 80)
	{
		this->coldMeter = -0.5;
		this->foodMeter = -0.5;
		this->waterMeter = -0.5;
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
	if (this->hp <= 100 && this->hp > 0)
		this->hp = this->hp - (this->damage * deltaTime);
	else if (this->hp > 100)
		this->hp = 100;

	// SPAWN & GAME OVER FADE
	if (this->startGame && this->fade > 0)
	{
		this->fade -= 0.005;
		if (this->fade <= 0)
			this->startGame = false;
	}

	if (this->hp <= 0 && this->fade < 1)
		this->fade += deltaTime;
		
	//Winning

	if (this->win == true)
	{
		this->flareTimer += deltaTime;
		if (flareTimer <= 10.0f)
		{
			this->winFade += deltaTime / 10.0f;
		}
	}

	// Text Fade
	if (this->textOnScreen == true)
		this->textFade -= 0.05;
	//else if (this->textOnScreen == false)
	//this->textFade = 1.0;

	if (cold <= 0.0f)
	{
		cold = 0.0f;
	}
	if (water <= 0.0f)
	{
		water = 0.0f;
	}
	if (food <= 0.0f)
	{
		food = 0.0f;
	}
}

void Player::processEvents(GLFWwindow * window, float deltaTime)
{

	isWalking = false;
	movingForward = false;
	movingBackwards = false;
	movingLeft = false;
	movingRight = false;

	lastPosTemp = Transformable::transform.position;

	useItem(window);

	//Equipment and Stats
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		setCold(10);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		setWater(10);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		setFood(10);
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		setCold(-10);
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		setWater(-10);
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		setFood(-10);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		hp -= 10;

	//Fast win
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		this->win = true;
	}

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		equip("EmptyImage");
		for (int i = 0; i < 5; i++)
			addImageToInventory("EmptyImage", i);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !isPressed && this->currentlyEquipedItem != 0)
	{
		if (inInventory[0] == true)
		{
			equip("AxeIcon");
			this->currentlyEquipedItem = 0;
			equipItemMesh();
			isPressed = true;
			swapItem = true;
			pullDown = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !isPressed && this->currentlyEquipedItem != 1)
	{
		equip("LighterIcon");
		this->currentlyEquipedItem = 1;
		equipItemMesh();
		addImageToInventory("InventoryLighterIcon", 1);
		inInventory[1] = true;
		isPressed = true;
		swapItem = true;
		pullDown = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !isPressed && this->currentlyEquipedItem != 2)
	{
		if (inInventory[2] == true)
		{
			equip("WoodIcon");
			this->currentlyEquipedItem = 2;
			equipItemMesh();
			isPressed = true;
			swapItem = true;
			pullDown = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !isPressed && this->currentlyEquipedItem != 3)
	{
		if (inInventory[3] == true)
		{
			equip("FoodIcon");
			this->currentlyEquipedItem = 3;
			equipItemMesh();
			addImageToInventory("InventoryFoodIcon", 3);
			inInventory[3] = true;
			isPressed = true;
			swapItem = true;
			pullDown = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !isPressed && this->currentlyEquipedItem != 4)
	{
		if (inInventory[4] == true)
		{
			equip("BucketIcon");
			this->currentlyEquipedItem = 4;
			equipItemMesh();
			isPressed = true;
			swapItem = true;
			pullDown = true;
		}
	}
	else if (  glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE
			&& glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE
			&& glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE)
	{
		isPressed = false;
	}

	//... Mouse Movement
	glfwGetCursorPos(window, &xpos, &ypos);

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw = xoffset;
	pitch = yoffset;

	glm::mat4 matrix = glm::mat4(1);


	oldYaw = oldYaw - yaw;
	oldPitch = oldPitch + pitch;

	matrix = glm::rotate(matrix, -oldYaw, Transformable::transform.up);
	glm::vec4 right = glm::vec4(matrix[0][0], matrix[1][0], matrix[2][0], 0);
	Transformable::transform.right = right;

	if (oldPitch < 1.48f && oldPitch > -1.48f)
	{
		matrix = glm::rotate(matrix, oldPitch, Transformable::transform.right);
		glm::vec4 forward = glm::vec4(matrix[0][2], matrix[1][2], matrix[2][2], 0);
		Transformable::transform.forward = forward;
	}
	else if (oldPitch > 1.48f)
	{
		oldPitch = 1.48f;
	}
	else if (oldPitch < -1.48f)
	{
		oldPitch = -1.48f;
	}

	if (firstMouse) {
lastX = (float)xpos;
lastY = (float)ypos;
firstMouse = false;
	}

	xoffset = (float)xpos - lastX;
	yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	//... WASD Movement
	glm::vec3 direction = glm::vec3(0);
	bool shift = false;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		shift = true;
		SnowCrunch.setPitch(1.5f);
		setWater(-0.01);
	}
	else
		SnowCrunch.setPitch(1.0);


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (jumpReady == true)
			isWalking = true;

		movingForward = true;


		direction += Transformable::transform.forward;
		if (shift == true)
			Transformable::transform.position += cameraSpeed * (Transformable::transform.forward * 1.8f) * deltaTime;
		else
			Transformable::transform.position += cameraSpeed * Transformable::transform.forward * deltaTime;
		//velocity
		Transformable::transform.velocity = Transformable::transform.forward * deltaTime * cameraSpeed;

	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (jumpReady == true)
			isWalking = true;

		movingBackwards = true;

		direction -= Transformable::transform.forward;
		Transformable::transform.position -= cameraSpeed * Transformable::transform.forward * deltaTime;
		//velocity
		Transformable::transform.velocity = Transformable::transform.forward * deltaTime * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (jumpReady == true)
			isWalking = true;

		movingLeft = true;

		direction += Transformable::transform.right;
		Transformable::transform.position += Transformable::transform.right * cameraSpeed * deltaTime;
		//velocity
		Transformable::transform.velocity = Transformable::transform.right * deltaTime * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		if (jumpReady == true)
			isWalking = true;

		movingRight = true;

		direction += Transformable::transform.right;
		Transformable::transform.position -= Transformable::transform.right * cameraSpeed * deltaTime;
		//velocity
		Transformable::transform.velocity = Transformable::transform.right * deltaTime * cameraSpeed;
	}

	//... Jump mechanic
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && inAir == false && jumpReady == true && !isColliding)
	{

		inAir = true;
		time = 0.0f;
		jumpReady = false;
	}


	if (movingForward || movingBackwards || movingLeft || movingRight)
		lastPos = lastPosTemp;
}
void  Player::swingTest()
{
	if (currentlyEquipedItem == 0 && !Swing.isPlaying())
	{
		Swing.playSound();
	}
}

void Player::eatFood()
{
	if (currentlyEquipedItem == 3 && inInventory[3] == true && foodTimer > 1.0f)
	{
		food += 50;
		if (food >= 100)
			food = 100;
		inInventory[3] = false;
		addImageToInventory("EmptyImage", 3);
		
	}
}

int Player::interactionResponse(const ObjectType::ID id, bool & isAlive, int & counter)
{
	// Check ObjectType ID
	// Set isAlive to false if you want to delete the interacted item from the world.


	if (id == ObjectType::ID::Axe)
	{
		if (inInventory[0] == false)
		{
			equip("Axe");
			this->currentlyEquipedItem = 0;
			this->equipItem = 33;
			addImageToInventory("InventoryAxeIcon", 0);
			inInventory[0] = true;
			isPressed = true;
			swapItem = true;
			pullDown = true;
			isAlive = false;
		}
		else
			addTextToScreen("Text-ItemAlreadyEquipped");
	}
	else if ((id == ObjectType::ID::BrokenTree || id == ObjectType::ID::BrokenTree_Snow || id == ObjectType::ID::DeadTree
		|| id == ObjectType::ID::DeadTreeSnow || id == ObjectType::ID::DeadTreeSnow_Small || id == ObjectType::ID::DeadTree_Small
		|| id == ObjectType::ID::Pine_Tree || id == ObjectType::ID::Pine_Tree_Snow || id == ObjectType::ID::Tree_Small || id == ObjectType::ID::Tree_Small_Snow)
		&& currentlyEquipedItem == 0)
	{
		if(!HitWAxe.isPlaying())
			HitWAxe.playSound();
		if (inInventory[2] == false)
		{
			if (counter >= 4)
			{
				equip("WoodIcon");
				this->currentlyEquipedItem = 2;
				addImageToInventory("InventoryWoodIcon", 2);
				inInventory[2] = true;
				this->currentlyEquipedItem = 2;
				this->equipItem = 45;
				swapItem = true;
				pullDown = true;
				isAlive = false;
			}
			else
				counter++;
		}
		else
			addTextToScreen("Text-ItemAlreadyEquipped");
	}
	else if (id == ObjectType::ID::Can)
	{
		if (inInventory[3] == false)
		{
			this->foodTimer = 0.0f;
			equip("FoodIcon");
			this->currentlyEquipedItem = 3;
			this->equipItem = 46;
			addImageToInventory("InventoryFoodIcon", 3);
			inInventory[3] = true;
			isPressed = true;
			swapItem = true;
			pullDown = true;
			isAlive = false;
		}
		else
			addTextToScreen("Text-ItemAlreadyEquipped");
	}
	else if (id == ObjectType::ID::Bucket_Empty)
	{
		if (inInventory[4] == false)
		{
			equip("BucketIcon");
			this->currentlyEquipedItem = 4;
			this->equipItem = 34;
			addImageToInventory("InventoryBucketIcon", 4);
			inInventory[4] = true;
			isPressed = true;
			swapItem = true;
			pullDown = true;
			isAlive = false;
		}
		else
			addTextToScreen("Text-ItemAlreadyEquipped");
	}
	else if (id == ObjectType::ID::Jacket)
	{
		jacket = true;
		equipItemMesh();
		swapItem = true;
		pullDown = true;
		isAlive = false;
		this->coldTick = 0.3;
	}
	else if (id == ObjectType::ID::Campfire && currentlyEquipedItem == 1)
	{
		return 3;
	}

	if (id == ObjectType::ID::FlareGun)
	{
		this->win = true;
		return 42;
	}
	else if(id == ObjectType::ID::Axe)
	{
		isAlive = false;
	}
	else if (id == ObjectType::ID::Axe)
	{
		isAlive = false;
	}
	/*
	if(id == fallenTree && axeIsEquiped)
	{
		logs++;
		isAlive = false;
	}
	*/

	return -1;
}

int Player::collisionResponse(const ObjectType::ID)
{
	if (id == ObjectType::ID::Campfire)
	{
		return 3;
	}

	return -1;
}

void Player::heatResponse()
{
	cold += .1f; 
	if (this->cold > 100)
		this->cold = 100;
}

void Player::takeDamange(float damage, float deltaTime)
{
	hp -= damage * deltaTime;
}

int Player::getEquipedItem()
{
	if (inInventory[2] == true) 
	{
		addImageToInventory("EmptyImage", 2);
		inInventory[2] = false;
		return 2;
	}

	return -1;
}

const int Player::getEquipedID()
{
	return this->equipedID;
}

void Player::equipItemMesh()
{
	switch (currentlyEquipedItem)
	{
		case 0:
		{
			if (jacket)
				this->equipItem = 50;
			else
				this->equipItem = 33;
			break;
		}
		case 1:
		{
			if (jacket)
				this->equipItem = 52;
			else
				this->equipItem = 44;
			break;
		}
		case 2:
		{
			if (jacket)
				this->equipItem = 53;
			else
				this->equipItem = 45;
			break;
		}
		case 3:
		{
			if (jacket)
				this->equipItem = 51;
			else
				this->equipItem = 46;
			break;
		}
		case 4:
		{
			if (jacket)
			{
				if (bucketContent = 0)
					this->equipItem = 47;
				else if (bucketContent = 1)
					this->equipItem = 48;
				else
					this->equipItem = 49;
			}
			else
			{
				if (bucketContent = 0)
					this->equipItem = 34;
				else if (bucketContent = 1)
					this->equipItem = 35;
				else
					this->equipItem = 36;
			}
		}
	}
		

}

void Player::findY()
{
	//float frontTemp = glm::mix(currentY, frontVertexHeight);
	int gridX = (int)glm::floor(cameraPos.x / distanceToNextVertex);
	int gridZ = (int)glm::floor(cameraPos.z / distanceToNextVertex);
	float gridSquareSize(distanceToNextVertex / ((float)vertexLength - 1));
	float xpos = cameraPos.x;
	float xCoord = ((int)cameraPos.x % (int)distanceToNextVertex) / distanceToNextVertex;
	float zCoord = ((int)cameraPos.z % (int)distanceToNextVertex) / distanceToNextVertex;
}

void Player::swingAxe(float deltaTime)
{
	if (currentlyEquipedItem != 0)
	{
		swing = false;
		return;
	}
	else if (pickUp < 0.4 && axeSwing == 0)
	{
		pickUp += deltaTime * 2;
		if (pickUp >= 0.4)
			axeSwing = 1;
	}
	else if (pickUp > -0.3 && axeSwing == 1)
	{
		pickUp -= deltaTime * 7;
		if (pickUp <= -0.3)
			axeSwing = 3;
	}
	else if (pickUp < 0 && axeSwing == 3)
	{
		pickUp += deltaTime;
		if (pickUp >= 0)
		{
			pickUp = 0;
			swing = false;
		}
	}
}

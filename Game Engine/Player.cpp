#include "Player.h"

Player::Player(Transform& transform) : Transformable(transform)
{
	Component::id = ComponentType::ID::PLAYER;

	click = false;
	addClick = false;

	this->currentlyEquipedItem = -1;
	this->equipedID = -1;

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
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0;
	yaw = 0;
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
	AmbientMusic.setVolume(50.0f);
	AmbientMusic.playSound();
	AmbientMusic.loop(true);

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
	if (height + 7.001 < currentY + 7.002);
		this->currentY = height + 7.0;
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

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !isPressed)
	{
		equip("AxeIcon");
		this->currentlyEquipedItem = 0;
		this->equipedID = 33;
		addImageToInventory("InventoryAxeIcon", 0);
		inInventory[0] = true;
		isPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !isPressed)
	{
		equip("LighterIcon");
		this->currentlyEquipedItem = 1;
		this->equipedID = 44;
		addImageToInventory("InventoryLighterIcon", 1);
		inInventory[1] = true;
		isPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !isPressed)
	{
		if (inInventory[2] == true)
		{
			equip("WoodIcon");
			isPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !isPressed)
	{
		equip("FoodIcon");
		this->currentlyEquipedItem = 3;
		addImageToInventory("InventoryFoodIcon", 3);
		inInventory[3] = true;
		isPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !isPressed)
	{
		equip("BucketIcon");
		this->currentlyEquipedItem = 4;
		this->equipedID = 34;
		addImageToInventory("InventoryBucketIcon", 4);
		inInventory[4] = true;
		isPressed = true;
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

	glm::vec4 forward = normalize(glm::vec4(Transformable::transform.forward, 0));
	glm::vec4 right = normalize(glm::vec4(Transformable::transform.right, 0));
	glm::vec4 up = normalize(glm::vec4(Transformable::transform.up, 0));

	matrix = glm::rotate(matrix, pitch, Transformable::transform.right);
	matrix = glm::rotate(matrix, -yaw, Transformable::transform.up);

	forward = matrix * forward;
	right = matrix * right;

//	up = glm::vec4(glm::cross(glm::vec3(right), glm::vec3(forward)), 0);
	//if ((glm::dot(Transformable::transform.forward, Transformable::transform.up) < 0.9f && pitch > 0.0f) || (glm::dot(Transformable::transform.forward, Transformable::transform.up) > -0.9f && pitch < 0.0f))
	Transformable::transform.forward = forward;
	//else
	//	pitch = 0;
//	Transformable::transform.up = up;
	Transformable::transform.right = right;



	//Transformable::transform.up = up;

	//printf("%f, %f, %f\n", Transformable::transform.right.x, Transformable::transform.right.y, Transformable::transform.right.z);
	//printf("%f,\n", glm::radians(pitch));

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


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && frontCollision == false)
	{
		if (jumpReady == true)
			isWalking = true;

		float tempY = Transformable::transform.position.y;
		direction += Transformable::transform.forward;
		if (shift == true)
			Transformable::transform.position += cameraSpeed * (Transformable::transform.forward * 1.8f) * deltaTime;
		else
			Transformable::transform.position += cameraSpeed * Transformable::transform.forward * deltaTime;
		Transformable::transform.position.y = tempY;
		//velocity
		Transformable::transform.velocity = Transformable::transform.forward * deltaTime * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && backCollision == false)
	{
		if (jumpReady == true)
			isWalking = true;

		float tempY = Transformable::transform.position.y;
		direction -= Transformable::transform.forward;
		Transformable::transform.position -= cameraSpeed * Transformable::transform.forward * deltaTime;
		Transformable::transform.position.y = tempY;
		//velocity
		Transformable::transform.velocity = Transformable::transform.forward * deltaTime * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && leftCollision == false)
	{
		if (jumpReady == true)
			isWalking = true;

		float tempY = Transformable::transform.position.y;
		direction -= Transformable::transform.right;
		Transformable::transform.position -= Transformable::transform.right * cameraSpeed * deltaTime;
		Transformable::transform.position.y = tempY;
		//velocity
		Transformable::transform.velocity = Transformable::transform.right * deltaTime * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && rightCollision == false)
	{
		if (jumpReady == true)
			isWalking = true;

		float tempY = Transformable::transform.position.y;
		direction += Transformable::transform.right;
		Transformable::transform.position += Transformable::transform.right * cameraSpeed * deltaTime;
		Transformable::transform.position.y = tempY;
		//velocity
		Transformable::transform.velocity = Transformable::transform.right * deltaTime * cameraSpeed;
	}

	//... Jump mechanic
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && inAir == false && jumpReady == true)
	{

		inAir = true;
		time = 0.0f;
		jumpReady = false;
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

	//if (Transformable::transform.position.y <= currentY -0.1f)
	//	Transformable::transform.position.y = currentY;
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
}

int Player::interactionResponse(const ObjectType::ID id, bool & isAlive)
{
	// Check ObjectType ID
	// Set isAlive to false if you want to delete the interacted item from the world.
	if (id == ObjectType::ID::Bucket_Empty)
	{
		equip("BucketIcon");
		addImageToInventory("InventoryBucketIcon", 4);
		isAlive = false;
	}
	else if (id == ObjectType::ID::Jacket)
	{
		isAlive = false;
	}
	else if (id == ObjectType::ID::Campfire && currentlyEquipedItem == 1)
	{
		return 3;
	}
	else if ((  id == ObjectType::ID::BrokenTree   || id == ObjectType::ID::BrokenTree_Snow    || id == ObjectType::ID::DeadTree
		|| id == ObjectType::ID::DeadTreeSnow || id == ObjectType::ID::DeadTreeSnow_Small || id == ObjectType::ID::DeadTree_Small
		|| id == ObjectType::ID::Pine_Tree    || id == ObjectType::ID::Pine_Tree_Snow     || id == ObjectType::ID::Tree 
		|| id == ObjectType::ID::TreeWithSnow || id == ObjectType::ID::Tree_Small         || id == ObjectType::ID::Tree_Small_Snow)
		&& currentlyEquipedItem == 0)
	{
		if (inInventory[2] == false)
		{
			isAlive = false;
			equip("WoodIcon");
			this->currentlyEquipedItem = 2;
			addImageToInventory("InventoryWoodIcon", 2);
			inInventory[2] = true;
		}
		else
			addTextToScreen("Text-ItemAlreadyEquipped");
	}
	if (id == ObjectType::ID::FlareGun)
	{
		this->win = true;
		return 42;
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

#include "Player.h"

Player::Player(Transform& transform) : Transformable(transform)
{
	Component::id = ComponentType::ID::PLAYER;

	click = false;

	this->hp = 50;
	this->cold = 100;
	this->coldMeter = 0;
	this->water = 100;
	this->waterMeter = 0;
	this->food = 100;
	this->foodMeter = 0;
	this->coldTick = 2;
	this->waterTick = 2;
	this->foodTick = 2;
	this->damage = 0;
	this->initializer = 0;
	this->textInitializer = 0;
	this->inventoryCount = 0;
	this->maxAmountOfItems = 5;
	this->fade = 1;
	this->textFade = 1;
	this->startGame = true;
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

	equip("EmptyImage");
	for (int i = 0; i < 5; i++)
		initiateInventoryTextures("EmptyImage");
	addTextToScreen("EmptyImageTexture");
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
		if (this->textTimer >= 1.0f ||this->textOnScreen == false)
		{
			std::cout << "Item already exists in players inventory" << std::endl;
			addTextToScreen("Text-ItemAlreadyEquipped");
		}
	}
	else
	{
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
	this->currentY = height;
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
	if (this->hp < 100 && this->hp > 0)
		this->hp = this->hp - (this->damage * deltaTime);

	// SPAWN & GAME OVER FADE
	if (this->startGame && this->fade > 0)
	{
		this->fade -= 0.005;
		if ( this->fade <= 0)
			this->startGame = false;
	}

	if (this->hp <= 0 && this->fade < 1)
		this->fade += deltaTime;

	// Text Fade
	if (this->textOnScreen == true)
		this->textFade -= 0.05;
	//else if (this->textOnScreen == false)
		//this->textFade = 1.0;
}

void Player::processEvents(GLFWwindow * window, float deltaTime)
{
	

	//Equipment and Stats
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		setCold(10);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		setWater(10);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		setFood(10);

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		equip("EmptyImage");
		for (int i = 0; i < 5; i++)
			addImageToInventory("EmptyImage", i);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		equip("AxeIcon");
		addImageToInventory("InventoryAxeIcon", 0);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		equip("LighterIcon");
		addImageToInventory("InventoryLighterIcon", 1);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		equip("WoodIcon");
		addImageToInventory("InventoryWoodIcon", 2);
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		equip("FoodIcon");
		addImageToInventory("InventoryFoodIcon", 3);
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		equip("BucketIcon");
		addImageToInventory("InventoryBucketIcon", 4);
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		equip("MainMenuConcept1");
	}


	//... Mouse Movement
	glfwGetCursorPos(window, &xpos, &ypos);

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw = xoffset;
	pitch = yoffset;

	if (pitch > 90.f)
		pitch = 90.0f;
	if (pitch < -90.0f)
		pitch = -90.0f;

	glm::mat4 matrix = glm::mat4(1);

	glm::vec4 forward = glm::vec4(Transformable::transform.forward, 0);
	glm::vec4 right = glm::vec4(Transformable::transform.right, 0);
	glm::vec4 up = glm::vec4(Transformable::transform.up, 0);

	matrix = glm::rotate(matrix, pitch, Transformable::transform.right);
	matrix *= glm::rotate(matrix, -yaw, Transformable::transform.up);

	forward = matrix * forward;
	up = matrix * up;
	right = matrix * right;

	Transformable::transform.forward.x = forward.x;
	Transformable::transform.forward.y = forward.y;
	Transformable::transform.forward.z = forward.z;

	Transformable::transform.right.x = right.x;
	Transformable::transform.right.y = right.y;
	Transformable::transform.right.z = right.z;

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
		shift = true;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && frontCollision == false)
	{
		float tempY = Transformable::transform.position.y;
		direction += Transformable::transform.forward;
		if(shift == true)
			Transformable::transform.position += cameraSpeed * (Transformable::transform.forward * 1.8f) * deltaTime;
		else
			Transformable::transform.position += cameraSpeed * Transformable::transform.forward * deltaTime;
		Transformable::transform.position.y = tempY;
		//velocity
		Transformable::transform.velocity = Transformable::transform.forward * deltaTime * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && backCollision == false)
	{
		float tempY = Transformable::transform.position.y;
		direction -= Transformable::transform.forward;
		Transformable::transform.position -= cameraSpeed * Transformable::transform.forward * deltaTime;
		Transformable::transform.position.y = tempY;
		//velocity
		Transformable::transform.velocity = Transformable::transform.forward * deltaTime * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && leftCollision == false)
	{
		float tempY = Transformable::transform.position.y;
		direction -= Transformable::transform.right;
		Transformable::transform.position -= Transformable::transform.right * cameraSpeed * deltaTime;
		Transformable::transform.position.y = tempY;
		//velocity
		Transformable::transform.velocity = Transformable::transform.right * deltaTime * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && rightCollision == false)
	{
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

//glm::mat4 Player::getViewMatrix() const
//{
//	return glm::lookAt(Transformable::transform.position, Transformable::transform.position + Transformable::transform.forward, Transformable::transform.up);
//}

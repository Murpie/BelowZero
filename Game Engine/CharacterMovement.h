#pragma once
#include <GL/gl3w.h>  
#include"Component.h"
#include "Physics.h"
#include<iostream>
#include <Windows.h>
#include "glfw/include/GLFW/glfw3.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"

class CharacterMovement :public Component
{
public:
	CharacterMovement();
	CharacterMovement(GLFWwindow* window);
	~CharacterMovement();

	void getInformation(float time);

	Physics physics;
	GLFWwindow* window;

	
	void update();
private:
	float jumpSpeed = 0.04;
	float cameraSpeed = 0.03;
	float fallSpeed = 0.07;

	bool frontCollision = false;
	bool bottomCollision = true;
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
	

	float time = 0.0f;
	bool gravity = true;
	//--------=====Jumping=====-----------

	bool jumpReady = true;
	bool inAir = false;
	float timeInAir = 0.4;
};


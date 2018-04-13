#pragma once
#include <GL/gl3w.h>  
//#include"Component.h"
#include "Transformable.h"
#include "Physics.h"
#include<iostream>
#include <Windows.h>
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"

class CharacterMovement : public Transformable
{
public:
	CharacterMovement(Transform& transform);
	~CharacterMovement();

	void getInformation(float time);

	Physics physics;

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


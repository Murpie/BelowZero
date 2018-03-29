#pragma once
#include <GL/gl3w.h>  
#include"Component.h"
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

	GLFWwindow* window;

	void update();
private:

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

};


#pragma once
#include <GL/gl3w.h>  
#include "glfw\include\GLFW\glfw3.h"
#include<string>

//class GameObject;

class Component
{
public:
	Component();
	virtual ~Component();

    //GameObject* gameObject;

	std::string assetName;
	bool operator==(const Component &rhs);
	virtual void setInformation(float time);

	virtual void update(float deltaTime) = 0;
	virtual void processEvents(GLFWwindow *window, float deltaTime) = 0;

private:

};

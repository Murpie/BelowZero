#pragma once
#include <vector>
#include "glm/glm/glm.hpp"

class Physics
{
private:
	float gravityForce = 9.8;
	glm::vec3 Drag = glm::vec3(0.9, 0.9, 0.9);

	bool gravityOn = true;
	glm::vec3 deltaDirection;
	float speedOfObject;

public:
	Physics();
	~Physics();

	glm::vec3 getInformation(glm::vec3 deltaDirection, float speed);

};
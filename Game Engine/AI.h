#pragma once
#include "Transformable.h"

#define MAX_DISTANCE 50
#define SPAWN_OFFSET 50
#define SPEED 5.0f

class AI :
	public Transformable
{
public:
	AI(Transform& transform);
	~AI();

	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);

	glm::vec2 getXY();
	void setCurrentHeight(float height);

	bool collision;

	//todo add function to rotate object towards the direction

private:
	glm::vec3 startPosition;
	glm::vec3 lastTarget;
	glm::vec3 target;
	glm::vec3 direction;

	float time;

	void swapDirection();

	void move(float deltaTime);
	void setNewTarget();
};


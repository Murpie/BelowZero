#pragma once
#include "Transformable.h"

#define MAX_DISTANCE 50
#define SPEED 2.0f
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

private:
	glm::vec3 startPosition;
	glm::vec3 lastTarget;
	glm::vec3 target;
	glm::vec3 direction;

	float time;

	void swapDirection();

	void move(float deltaTime);
	void moveHome();
	void rotate(float deltaTime);

	void awake();
	void wander();
	void setNewTarget();
};


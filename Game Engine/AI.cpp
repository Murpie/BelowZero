#include "AI.h"



AI::AI(Transform& transform) : Transformable(transform)
{
	Component::id = ComponentType::ID::AI;

	this->startPosition = transform.position;
	this->collision = false;
	this->time = 0.f;

	lastTarget = glm::vec3(0, 0, 0);
	target = glm::vec3(0, 0, 0);
	direction = glm::vec3(0, 0, 0);
}


AI::~AI()
{

}

void AI::update(float deltaTime, float seconds)
{
	time += deltaTime;

	if (time > 10.f || collision)
	{
		setNewTarget();
	}
	if (time > 3.f && time < 5.f)
	{

	}
	else
		move(deltaTime);
}

void AI::processEvents(GLFWwindow * window, float deltaTime)
{

}

glm::vec2 AI::getXY()
{
	float u = Transformable::transform.position.x;
	float v = Transformable::transform.position.z;


	return glm::vec2(u, v);
}

void AI::setCurrentHeight(float height)
{
	if (height + 0.001 < Transformable::transform.position.y + 0.002);
	this->Transformable::transform.position.y = height + 0.0;
}

void AI::swapDirection()
{

}

void AI::move(float deltaTime)
{
	Transformable::transform.position +=  direction * (float)SPEED * deltaTime;
}

void AI::moveHome()
{

}

void AI::rotate(float deltaTime)
{
	transform.rotation.y += 10.f * deltaTime;
	Transformable::transform.forward = transform.rotation;
}

void AI::awake()
{

}

void AI::wander()
{
}

void AI::setNewTarget()
{
	glm::vec3 currentPosition = Transformable::transform.position;
	lastTarget = target;
	float x = rand() % 20 - 10;
	float z = rand() % 20 - 10;
	// Update target
	target = glm::vec3(startPosition.x + x, startPosition.y, startPosition.z + z);
	// Update direction
	direction = glm::normalize(glm::vec3(target.x - currentPosition.x, target.y - currentPosition.y, target.z - currentPosition.z));
	// Reset timer
	time = 0.f;
}

/*

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
*/
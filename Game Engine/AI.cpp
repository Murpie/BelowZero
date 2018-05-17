#include "AI.h"



AI::AI(Transform& transform) : Transformable(transform)
{
	Component::id = ComponentType::ID::AI;
	this->startPosition = transform.position;
	this->collision = false;
	this->direction = 1.f;
}


AI::~AI()
{
}

void AI::update(float deltaTime, float seconds)
{

	if (collision)
	{
		rotate(deltaTime);
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
	if (height + 1.001 < Transformable::transform.position.y + 1.002);
	this->Transformable::transform.position.y = height + 1.0;
}

void AI::swapDirection()
{
	if (direction == 1)
		direction = -1;
	else
		direction = 1;
}

void AI::move(float deltaTime)
{
	//Transformable::transform.position;
	Transformable::transform.position += direction * transform.forward * (float)SPEED * deltaTime;
	//X: = originX + cos(angle)*radius;
	//Y: = originY + sin(angle)*radius;
	// (originX, originY) is the center of your circle. radius is its radius. That's it.
	float radius = 20.f;
	float x = startPosition.x + glm::cos(30) * radius;
	float y = startPosition.y + glm::sin(30) * radius;

	//Transformable::transform.position.x = x * (float)SPEED; //* deltaTime;
	//Transformable::transform.position.y += y * (float)SPEED * deltaTime;
}

void AI::moveHome()
{

}

void AI::rotate(float deltaTime)
{
	transform.rotation.y += 10.f * deltaTime;
	Transformable::transform.forward = transform.rotation;
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
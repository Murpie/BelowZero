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
	forward = Transformable::transform.right;
}


AI::~AI()
{

}

void AI::update(float deltaTime, float seconds)
{
	time += deltaTime;

	if (time > 8.f || collision)
	{
		setNewTarget();
	}
	//if (time > 3.f && time < 5.f)
	//{

	//}
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

void AI::setNewTarget()
{
	glm::vec3 currentPosition = Transformable::transform.position;
	lastTarget = target;
	// Get new target coords
	float x = rand() % SPAWN_OFFSET - SPAWN_OFFSET * 0.5;
	float z = rand() % SPAWN_OFFSET - SPAWN_OFFSET * 0.5;
	// Update target
	target = glm::vec3(startPosition.x + x, startPosition.y, startPosition.z + z);
	// Direction
	direction = glm::normalize(target - currentPosition);
	// Reset timer
	time = 0.f;
	// Calculate angle
	float dot = glm::dot(forward, direction);
	float directionMag = glm::length(direction);
	float forwardMag = glm::length(forward);

	float angle = dot / (directionMag * forwardMag);
	angle = glm::degrees(glm::acos(angle));

	std::cout << "Rabbit Angle: " << angle << std::endl;
	std::cout << "Rabbit Direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;

	if (direction.x > 0.f && direction.z > 0.f)
		Transformable::transform.rotation.y = angle * -1; // tweak
	else if (direction.x < 0.f && direction.z > 0.f) 
		Transformable::transform.rotation.y = angle	; // tweak
	else if (direction.x > 0.f && direction.z > 0.f)
		Transformable::transform.rotation.y = angle * -1;
	else
		Transformable::transform.rotation.y = angle;
}
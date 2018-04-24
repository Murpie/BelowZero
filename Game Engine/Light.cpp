#include "Light.h"

Light::Light(Transform& transform): Transformable(transform)
{
	assetName = "Light";

	color = glm::vec4(1, 0.9, 0.7, 1);
	Linear=0.05;
	Quadratic=0.15;
	lightType = -1;
}

Light::~Light()
{
}

void Light::update(float deltaTime, float seconds)
{
}

void Light::processEvents(GLFWwindow * window, float deltaTime)
{
}
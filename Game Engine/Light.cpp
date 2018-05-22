#include "Light.h"

Light::Light(Transform& transform): Transformable(transform)
{
	assetName = "Light";

	color = glm::vec4(0.85, 0.85, 1.0, 1);
	Linear=0.05;
	Quadratic=0.15;
	lightType = -1;
	offset = 0;
	intensity = 1;
	isLighter = false;
	isFlare = false;
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
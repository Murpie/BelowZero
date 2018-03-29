#include "Light.h"

Light::Light():Component()
{
	assetName = "Light";

	color = glm::vec4(1, 0.9, 0.7, 1);
	Linear=0.05;
	Quadratic=0.15;
}

Light::~Light()
{
}

void Light::update()
{
}

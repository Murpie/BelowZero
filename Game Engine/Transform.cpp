#include "Transform.h"

Transform::Transform(): Component()
{
	assetName = "Transform";

	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);
}

Transform::Transform(glm::vec3 otherPosition)
{
	assetName = "Transform";
	position = otherPosition;
	rotation = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);
}

Transform::~Transform()
{
}

void Transform::update()
{
}

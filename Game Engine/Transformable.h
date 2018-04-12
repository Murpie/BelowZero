#pragma once
#include "Component.h"

#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"

struct Transform
{
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);

	glm::vec3 forward = glm::vec3(1, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 right = glm::vec3(0, 0, 1);
};

class Transformable : public Component
{
public:
	Transformable(Transform& transform);
	virtual ~Transformable();

	Transform& transform;
};


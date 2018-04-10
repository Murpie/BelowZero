#pragma once
#include <GL/gl3w.h>  
#include"Component.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"

class Light :public Component
{
public:
	Light();
	~Light();

	glm::vec4 color;
	int lightType;
	float Linear;
	float Quadratic;
	void update(float deltaTime);

private:

};


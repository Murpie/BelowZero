#pragma once
#include <GL/gl3w.h>
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"

class VFX
{
public:
	VFX();
	~VFX();

	struct Particle {
		glm::vec3 pos, speed;
		unsigned char r, g, b, a; // Color
		float size, angle, weight, cameraDistance;
		float life; // Remaining life of the particle. if < 0 : dead and unused.
	};
};


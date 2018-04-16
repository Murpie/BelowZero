#pragma once

#include "glm/glm/gtc/matrix_transform.hpp"

// remove this when we can import
struct vertex
{

};

struct bBox
{

};
//--------------------------------

class Intersection
{
	Intersection() = delete;
	Intersection(const Intersection&) = delete;
	Intersection& operator=(const Intersection&) = delete;

	static bool rayBoxTest(const glm::vec3& ray, const bBox& bBox)
	{

	}

	static bool collisionTest(const bBox& bBox1, const bBox& bBox2)
	{

	}
};
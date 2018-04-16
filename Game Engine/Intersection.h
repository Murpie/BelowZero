#pragma once

#include "glm/glm/gtc/matrix_transform.hpp"

// remove this when we can import
struct bBox
{
	bBox()
	{
		verts[0] = glm::vec3(5, 5, 5);
		verts[1] = glm::vec3(5, 5, 0);
		verts[2] = glm::vec3(5, 0, 0);
		verts[3] = glm::vec3(0, 0, 0);
		verts[4] = glm::vec3(0, 5, 5);
		verts[5] = glm::vec3(0, 0, 5);
		verts[6] = glm::vec3(5, 0, 5);
		verts[7] = glm::vec3(0, 5, 0);

		vMin = verts[0];
		vMax = verts[0];

		for (int i = 1; i < 8; i++)
		{
			//Min
			if (vMin.x > verts[i].x)
				vMin.x = verts[i].x;
			if (vMin.y > verts[i].y)
				vMin.y = verts[i].y;
			if (vMin.z > verts[i].z)
				vMin.z = verts[i].z;
			//Max
			if (vMax.x < verts[i].x)
				vMax.x = verts[i].x;
			if (vMax.y < verts[i].y)
				vMax.y = verts[i].y;
			if (vMax.z < verts[i].z)
				vMax.z = verts[i].z;
		}

		center = glm::vec3((vMin.x + vMax.x) * 0.5f, (vMin.y + vMax.y) * 0.5f, (vMin.z + vMax.z) * 0.5f);
	}

	glm::vec3 vMin; // smallest possible vector
	glm::vec3 vMax; // biggest possible vector

	glm::vec3 center; // center point of the bBox

	glm::vec3 verts[8];
};
//--------------------------------

class Intersection
{
public:
	Intersection() = delete;
	Intersection(const Intersection&) = delete;
	Intersection& operator=(const Intersection&) = delete;

	static bool rayBoxTest(const glm::vec3& rayDirection, const glm::vec3& rayOrigin, const bBox& bBox)
	{
		/*
		RayOrigin, RayDirection
		*/
		glm::vec3 dirfrac;
		float t;
		// rayDirection is unit direction vector of ray
		/*
		dirfrac.x = 1.0f / rayDirection.x;
		dirfrac.y = 1.0f / rayDirection.y;
		dirfrac.z = 1.0f / rayDirection.z;
		*/

		// vMin is the corner of AABB with minimal coordinates, vMax is maximal corner
		// rayOrigin is origin of ray.
		float t1 = (bBox.vMin.x - rayOrigin.x)*rayDirection.x;
		float t2 = (bBox.vMax.x - rayOrigin.x)*rayDirection.x;
		float t3 = (bBox.vMin.y - rayOrigin.y)*rayDirection.y;
		float t4 = (bBox.vMax.y - rayOrigin.y)*rayDirection.y;
		float t5 = (bBox.vMin.z - rayOrigin.z)*rayDirection.z;
		float t6 = (bBox.vMax.z - rayOrigin.z)*rayDirection.z;

		float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
		float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

		//if tmax < 0, ray(line) is intersecting AABB, but the whole AABB is behind us
		if (tmax < 0)
		{
			t = tmax;
			return false;
		}
		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
		{
			t = tmax;
			return false;
		}
		t = tmin;
		return true;

		/*
		If this returns true, it's intersecting, if it returns false, it's not intersecting.

		If you use the same ray many times, you can precompute dirfrac
		(only division in whole intersection test). And then it's really fast.
		And you have also length of ray until intersection (stored in  t).
		*/
	}

	static bool collisionTest(const bBox& bBox1, const bBox& bBox2)
	{

	}
};
#pragma once

#include <limits>

#include "Ray.h"

// remove this when we can import
struct bBox
{
	glm::vec3 vMin; // smallest possible vector
	glm::vec3 vMax; // largest possible vector
	glm::vec3 center; // center point of the bBox
};
//--------------------------------

class Intersection
{
public:
	Intersection() = delete;
	Intersection(const Intersection&) = delete;
	Intersection& operator=(const Intersection&) = delete;

	static bool rayBoxTest(const RayData rayData, const bBox& bBox, const glm::mat4 modelMatrix)
	{
		//glm::mat4 modelMatrix = glm::mat4(1); // get this from gameObject[i]?

		float tMin = -1000.f;
		float tMax = 1000.f;

		glm::vec3 p = bBox.center - rayData.rayOrigin;

		glm::vec3 OBBposition_worldspace(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);
		glm::vec3 delta = OBBposition_worldspace - rayData.rayOrigin;
	
		// Test intersection with the 2 planes perpendicular to the OBB's X axis
		glm::vec3 xAxis(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z);
		float e = glm::dot(xAxis, delta);
		float f = glm::dot(rayData.rayDirection, xAxis);

		if (glm::abs(f) > 0.001f) // Standard case
		{
			float t1 = (e + bBox.vMin.x) / f; // Intersection with the "left" plane
			float t2 = (e + bBox.vMax.x) / f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2

			if (t1>t2) 
			{
				// swap t1 and t2
				float w = t1; 
				t1 = t2; 
				t2 = w; 
			}
			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;
			// If "far" is closer than "near", then there is NO intersection.
			if (tMax < tMin)
				return false;
		}
		else {
			// Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + bBox.vMin.x > 0.0f || -e + bBox.vMax.x < 0.0f)
				return false;
		} 	
		// Test intersection with the 2 planes perpendicular to the OBB's Y axis
		// Exactly the same thing as above.
		glm::vec3 yAxis(modelMatrix[1].x, modelMatrix[1].y, modelMatrix[1].z);
		e = glm::dot(yAxis, delta);
		f = glm::dot(rayData.rayDirection, yAxis);

		if (abs(f) > 0.001f)
		{
			float t1 = (e + bBox.vMin.y) / f; // Intersection with the "left" plane
			float t2 = (e + bBox.vMax.y) / f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2

			if (t1>t2)
			{
				// swap t1 and t2
				float w = t1;
				t1 = t2;
				t2 = w;
			}
			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;
			// If "far" is closer than "near", then there is NO intersection.
			if (tMax < tMin)
				return false;
		}
		else {
			if (-e + bBox.vMin.y > 0.0f || -e + bBox.vMax.y < 0.0f)
				return false;
		}
		// Test intersection with the 2 planes perpendicular to the OBB's Z axis
		// Exactly the same thing than above.
		glm::vec3 zAxis(modelMatrix[2].x, modelMatrix[2].y, modelMatrix[2].z);
		e = glm::dot(zAxis, delta);
		f = glm::dot(rayData.rayDirection, zAxis);
		if (abs(f) > 0.001f)
		{
			float t1 = (e + bBox.vMin.z) / f; // Intersection with the "left" plane
			float t2 = (e + bBox.vMax.z) / f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2

			if (t1>t2)
			{
				// swap t1 and t2
				float w = t1;
				t1 = t2;
				t2 = w;
			}
			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;
			// If "far" is closer than "near", then there is NO intersection.
			if (tMax < tMin)
				return false;
		}
		else {
			if (-e + bBox.vMin.z > 0.0f || -e + bBox.vMax.z < 0.0f)
				return false;
		}

		//intersection_distance = tMin;
		return true;

		/*
				// The ModelMatrix transforms :
				// - the mesh to its desired position and orientation
				// - but also the AABB (defined with aabb_min and aabb_max) into an OBB
				glm::mat4 RotationMatrix = glm::toMat4(orientations[i]);
				glm::mat4 TranslationMatrix = translate(mat4(), positions[i]);
				glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix;
		*/
	}

	static bool collisionTest(const bBox& bBox1, glm::vec3& position1, const bBox& bBox2, glm::vec3& posittion2)
	{
		//bBox1
		bBox temp1 = bBox1;
		temp1.vMax += position1;
		temp1.vMin += position1;
		//bBox2
		bBox temp2 = bBox2;
		temp2.vMax += posittion2;
		temp2.vMin += posittion2;
		//Collision test
		return(
			temp1.vMax.x > temp2.vMin.x &&
			temp1.vMin.x < temp2.vMax.x &&
			temp1.vMax.y > temp2.vMin.y &&
			temp1.vMin.y < temp2.vMax.y &&
			temp1.vMax.z > temp2.vMin.z &&
			temp1.vMin.z < temp2.vMax.z);
	}

	static float sweptAABB(bBox b1, bBox b2, float& normalX, float& normalY, float& normalZ, Transform* player)
	{
		// b1 is a moving box -> the player
		// b2 is a static box

		float xInvEntry, yInvEntry, zInvEntry;
		float xInvExit, yInvExit, zInvExit;
		//find the distance between the objects on the near and far sides for x, y and z
		// x-axis
		if (player->velocity.x > 0.0f)
		{
			xInvEntry = b2.vMin.x - b1.vMax.x;
			xInvExit = b2.vMax.x - b1.vMin.x;
		}
		else
		{
			xInvEntry = b2.vMax.x - b1.vMin.x;
			xInvExit = b2.vMin.x - b1.vMax.x;
		}
		// y-axis
		if (player->velocity.y > 0.0f)
		{
			yInvEntry = b2.vMin.y - b1.vMax.y;
			yInvExit = b2.vMax.y - b1.vMin.y;
		}
		else
		{
			yInvEntry = b2.vMax.y - b1.vMin.y;
			yInvExit = b2.vMin.y - b1.vMax.y;
		}
		// z-axis
		if (player->velocity.x > 0.0f)
		{
			zInvEntry = b2.vMin.z - b1.vMax.z;
			zInvExit = b2.vMax.z - b1.vMin.z;
		}
		else
		{
			zInvEntry = b2.vMax.z - b1.vMin.z;
			zInvExit = b2.vMin.z - b1.vMax.z;
		}

		// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
		float xEntry, yEntry, zEntry;
		float xExit, yExit, zExit;
		//x-axis
		if (player->velocity.x == 0.0f)
		{
			xEntry = -std::numeric_limits<float>::infinity();
			xExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			xEntry = xInvEntry / player->velocity.x;
			xExit = xInvExit / player->velocity.x;
		}
		//y-axis
		if (player->velocity.y == 0.0f)
		{
			yEntry = -std::numeric_limits<float>::infinity();
			yExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			yEntry = yInvEntry / player->velocity.y;
			yExit = yInvExit / player->velocity.y;
		}
		//z-axis
		if (player->velocity.z == 0.0f)
		{
			zEntry = -std::numeric_limits<float>::infinity();
			zExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			xEntry = xInvEntry / player->velocity.x;
			xExit = xInvExit / player->velocity.x;
		}

		// find the earliest/latest times of collision
		float entryTime = glm::max(xEntry, yEntry);
		float exitTime = glm::min(xExit, yExit);

		// if there was no collision
		if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f  && zEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f || zEntry > 1.0f)
		{
			normalX = 0.0f;
			normalY = 0.0f;
			normalZ = 0.0f;
			return 1.0f;
		}
		else // if there was a collision
		{
			// rewrite this code to 3D... 
			// https://www.gamedev.net/articles/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/
			
			if (xEntry > yEntry && xEntry > zEntry)
			{
				if (xInvEntry < 0.0f)
				{
					normalX = 1.0f;
					normalY = 0.0f;
					normalZ = 0.0f;
				}
				else
				{
					normalX = -1.0f;
					normalY = 0.0f;
					normalZ = 0.0f;
				}
			}
			else if (yEntry > xEntry && yEntry > zEntry)
			{
				if (yInvEntry < 0.0f)
				{
					normalX = 0.0f;
					normalY = 1.0f;
					normalZ = 0.0f;
				}
				else
				{
					normalX = 0.0f;
					normalY = -1.0f;
					normalZ = 0.0f;
				}
			}
			else
			{
				if (zInvEntry < 0.0f)
				{
					normalX = 0.0f;
					normalY = 0.0f;
					normalZ = 1.0f;
				}
				else
				{
					normalX = 0.0f;
					normalY = 0.0f;
					normalZ = -1.0f;
				}
			}
		}
		return entryTime;
	}
};
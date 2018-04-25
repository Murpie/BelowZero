#pragma once

#include "Ray.h"

// remove this when we can import
struct bBox
{
	bBox()
	{
		/*
		glm::mat4 worldMatrix = glm::mat4();
		// bucket maya coords from c
		verts[0] = glm::vec3(-0.992, 0.000, 1.064);
		verts[1] = glm::vec3(0.992, 0.000, 1.064);
		verts[2] = glm::vec3(-0.992, 2.398, 1.064);
		verts[3] = glm::vec3(0.992, 2.398, 1.064);
		verts[4] = glm::vec3(-0.992, 2.398, -1.064);
		verts[5] = glm::vec3(0.992, 2.398, -1.064);
		verts[6] = glm::vec3(-0.992, 0.000, -1.064);
		verts[7] = glm::vec3(0.992, 0.000, -1.064);

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
		*/
	}

	glm::vec3 vMin; // smallest possible vector
	glm::vec3 vMax; // largest possible vector

	glm::vec3 center; // center point of the bBox

	//glm::vec3 verts[8];

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
		// Exactly the same thing than above.
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

	static bool collisionTest(const bBox& bBox1, const bBox& bBox2)
	{
		return(
			bBox1.vMax.x > bBox2.vMin.x &&
			bBox1.vMin.x < bBox2.vMax.x &&
			bBox1.vMax.y > bBox2.vMin.y &&
			bBox1.vMin.y < bBox2.vMax.y &&
			bBox1.vMax.z > bBox2.vMin.z &&
			bBox1.vMin.z < bBox2.vMax.z);
	}
};
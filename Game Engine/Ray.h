#pragma once

#include "glm/glm/gtc/matrix_transform.hpp"



class Ray
{
public:
	Ray() = delete;
	Ray(const Ray&) = delete;
	Ray& operator=(const Ray&) = delete;

	//
	static glm::vec3 getWorldRay(const float x, const float y, glm::mat4 viewMatrix, float sceenWidth, float screenHeight)
	{
		float xPos = x;
		float yPos = y;

		//? Transform our x and y position into normalized device coordinates
		xPos = (2.f * xPos) / sceenWidth - 1.f;
		yPos = 1.f - (2.f * yPos) / screenHeight; // Reverse the direction of Y
		glm::vec2 normalizedCoords(xPos, yPos);
		
		//? Make our ray's z to point forward
		glm::vec4 clipCoords(xPos, yPos, -1.f, 1.f);
			
		//? Transform to eye space
		//? Multiply with the inverse of the matrix
		glm::mat4 projectionMatrix;
		glm::vec4 eyeCoords = glm::inverse(projectionMatrix) * clipCoords;
		eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.f, 0.f);
		
		//? Transform to world space
		glm::vec4 rayWorld = (glm::inverse(viewMatrix) * eyeCoords);
		glm::vec3 mouseRay = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);
		mouseRay = glm::normalize(mouseRay);

		return mouseRay;
	}
};
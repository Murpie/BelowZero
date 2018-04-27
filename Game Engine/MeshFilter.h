#pragma once
#include <GL/gl3w.h>  
#include"Component.h"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include "glm\glm\glm.hpp"


class MeshFilter :public Component
{
public:
	MeshFilter();
	MeshFilter(GLuint gVertexBuffer, GLuint gVertexAttribute, int otherVertexCount, int meshType);
	~MeshFilter();

	GLuint gVertexBuffer;
	GLuint gVertexAttribute;
	int vertexCount;
	int meshType;
	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);

	glm::mat4 worldTransform = glm::mat4(1.0f);

	void bindVertexArray();
private:

};


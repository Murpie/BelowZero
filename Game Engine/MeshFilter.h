#pragma once
#include <GL/gl3w.h>  
#include"Component.h"

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

	void bindVertexArray();
private:

};


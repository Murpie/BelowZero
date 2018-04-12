#pragma once
#include <GL/gl3w.h>  
#include"Component.h"

class MeshFilter :public Component
{
public:
	MeshFilter();
	MeshFilter(GLuint gVertexBuffer, GLuint gVertexAttribute, GLuint gElementBuffer, int otherVertexCount);
	~MeshFilter();

	GLuint gVertexBuffer;
	GLuint gVertexAttribute;
	GLuint gElementBuffer;
	int vertexCount;

	void update(float deltaTime);
	void processEvents(GLFWwindow *window, float deltaTime);

	void bindVertexArray();
private:

};


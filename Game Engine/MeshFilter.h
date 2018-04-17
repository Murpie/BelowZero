#pragma once
#include <GL/gl3w.h>  
#include"Component.h"

class MeshFilter :public Component
{
public:
	MeshFilter();
	MeshFilter(GLuint gVertexBuffer, GLuint gVertexAttribute, GLuint gElementBuffer, int otherVertexCount, int meshType);
	~MeshFilter();

	GLuint gVertexBuffer;
	GLuint gVertexAttribute;
	GLuint gElementBuffer;
	int vertexCount;
	int meshType;
	void update(float deltaTime);
	void processEvents(GLFWwindow *window, float deltaTime);

	void bindVertexArray();
private:

};


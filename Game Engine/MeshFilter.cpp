#include "MeshFilter.h"

MeshFilter::MeshFilter():Component()
{
	assetName = "MeshFilter";

}

MeshFilter::MeshFilter(GLuint otherVertexBuffer, GLuint otherVertexAttribute, GLuint otherElementBuffer, int otherVertexCount)
{
	assetName = "MeshFilter";
	gVertexBuffer = otherVertexBuffer;
	gVertexAttribute=otherVertexAttribute;
	gElementBuffer=otherElementBuffer;
	vertexCount = otherVertexCount;
}

MeshFilter::~MeshFilter()
{
}

void MeshFilter::update(float deltaTime, float seconds)
{
}

void MeshFilter::processEvents(GLFWwindow * window, float deltaTime)
{

}

void MeshFilter::bindVertexArray()
{
	glBindVertexArray(gVertexAttribute);
}


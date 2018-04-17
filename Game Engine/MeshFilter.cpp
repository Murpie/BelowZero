#include "MeshFilter.h"

MeshFilter::MeshFilter():Component()
{
	assetName = "MeshFilter";

}

MeshFilter::MeshFilter(GLuint otherVertexBuffer, GLuint otherVertexAttribute, GLuint otherElementBuffer, int otherVertexCount, int meshType )
{
	assetName = "MeshFilter";
	gVertexBuffer = otherVertexBuffer;
	gVertexAttribute=otherVertexAttribute;
	gElementBuffer=otherElementBuffer;
	vertexCount = otherVertexCount;
	this->meshType = meshType;
}

MeshFilter::~MeshFilter()
{
}

void MeshFilter::update(float deltaTime)
{
}

void MeshFilter::processEvents(GLFWwindow * window, float deltaTime)
{

}

void MeshFilter::bindVertexArray()
{
	glBindVertexArray(gVertexAttribute);
}


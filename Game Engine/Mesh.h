#pragma once
// macro that returns "char*" with offset "i"
// BUFFER_OFFSET(5) transforms in "(char*)nullptr+(5)"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))


#include <windows.h>
#include <string>
#include <GL/gl3w.h>  
#include "Vertex.h"
#include <vector>
#include <iostream>
#include "glm/glm/glm.hpp"
#include <algorithm>    // std::unique, std::distance

class Mesh
{
public:
	Mesh();
	Mesh(std::string filePath, GLuint gShaderProgram);
	~Mesh();

	void CreateMeshData(std::string filePath, GLuint gShaderProgram);

	std::vector<Vertex> Vertices;
	std::vector<unsigned int> indices;
	int vertexCount;
	GLuint gVertexBuffer;
	GLuint gVertexAttribute;
	GLuint gElementBuffer;
};


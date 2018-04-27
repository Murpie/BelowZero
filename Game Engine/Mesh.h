#pragma once
// macro that returns "char*" with offset "i"
// BUFFER_OFFSET(5) transforms in "(char*)nullptr+(5)"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))


#include <windows.h>
#include <string>
#include <GL/gl3w.h>  
#include "Vertex.h"
#include "LeapImporter.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "glm/glm/glm.hpp"
#include <algorithm>    // std::unique, std::distance
#include "Terrain.h"

class Mesh
{
public:
	Mesh();
	Mesh(std::string filePath, GLuint gShaderProgram, Terrain * terrain);
	~Mesh();

	void CreateMeshData(std::string filePath, GLuint gShaderProgram, Terrain *terrain);

	//std::vector<Vertex> Vertices;
	//std::vector<unsigned int> indices;

	LeapMesh* leapMesh;
	//int vertexCount;
	GLuint meshType; //remove this later and use *leapMesh
	GLuint gVertexBuffer;
	GLuint gVertexAttribute;
};


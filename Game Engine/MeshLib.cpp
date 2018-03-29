#include "MeshLib.h"

MeshLib::MeshLib()
{
}

MeshLib::~MeshLib()
{
}

void MeshLib::addMesh(std::string filePath, GLuint gShaderProgram)
{
	Mesh newMesh = Mesh(filePath, gShaderProgram);
	meshes.push_back(newMesh);
}

 Mesh & MeshLib::getMesh(int key)
{
	return meshes.at(key);
}

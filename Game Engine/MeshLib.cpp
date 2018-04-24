#include "MeshLib.h"

MeshLib::MeshLib()
{
}

MeshLib::~MeshLib()
{
}

void MeshLib::addMesh(std::string filePath, GLuint gShaderProgram, GLuint meshType, Terrain * terrain)
{
	Mesh newMesh = Mesh(filePath, gShaderProgram, meshType, terrain);
	meshes.push_back(newMesh);
}

void MeshLib::addAnimatedMesh(std::string filePath, GLuint gShaderProgram)
{
	AnimatedMesh newAnimMesh = AnimatedMesh(filePath, gShaderProgram);
	animatedMeshes.push_back(newAnimMesh);
}

Mesh & MeshLib::getMesh(int key)
{
	return meshes.at(key);
}

AnimatedMesh & MeshLib::getAnimatedMesh(int key)
{
	return animatedMeshes.at(key);
}

int MeshLib::getNumberOfMeshes()
{
	return meshes.size();
}

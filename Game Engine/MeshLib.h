#pragma once
#include<vector>
#include"Mesh.h"
#include<string>


class MeshLib
{
public:
	MeshLib();

	~MeshLib();

	void addMesh(std::string filePath, GLuint gShaderProgram);
	Mesh& getMesh(int key);

private:
	std::vector<Mesh> meshes;
};
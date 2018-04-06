#pragma once
#include<vector>
#include"Mesh.h"
#include"AnimatedMesh.h"
#include<string>


class MeshLib
{
public:
	MeshLib();

	~MeshLib();

	void addMesh(std::string filePath, GLuint gShaderProgram);
	void addAnimatedMesh(std::string filePath, GLuint gShaderProgram);
	Mesh& getMesh(int key);
	AnimatedMesh& getAnimatedMesh(int key);


private:
	std::vector<Mesh> meshes;
	std::vector<AnimatedMesh> animatedMeshes;
};
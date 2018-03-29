#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include <GL/gl3w.h>  
#include "ShaderBase.h"
#include "Mesh.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")


class SSAOShaders :public ShaderBase
{
public:
	SSAOShaders();
	SSAOShaders(int otherAssetID);
	~SSAOShaders();

	void CreateShaderData();

	//GLuint gVertexBuffer;
	//GLuint gVertexAttribute;
	//GLuint gElementBuffer;
	GLuint ssaoShaderProgram;
	int assetID;

};


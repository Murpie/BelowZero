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


class SSAOBlurShaders :public ShaderBase
{
public:
	SSAOBlurShaders();
	SSAOBlurShaders(int otherAssetID);
	~SSAOBlurShaders();

	void CreateShaderData();

	//GLuint gVertexBuffer;
	//GLuint gVertexAttribute;
	//GLuint gElementBuffer;
	GLuint ssaoBlurShaderProgram;
	int assetID;

};


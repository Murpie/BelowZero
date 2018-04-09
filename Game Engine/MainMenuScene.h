#ifndef MAINMENUSCENE_H
#define MAINMENUSCENE_H
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include <GL/gl3w.h>  
#include "ShaderBase.h"
#include "Mesh.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

class MainMenu : public ShaderBase
{
public:
	MainMenu();
	MainMenu(int otherAssetID);
	~MainMenu();

	void CreateShaderData();

	
	GLuint mainMenuShaderProgram;
	int assetID;

};
#endif // !MAINMENUSCENE_H

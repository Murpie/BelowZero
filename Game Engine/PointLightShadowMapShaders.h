#ifndef POINTLIGHTSHADOWMAPSHADERS_H
#define POINTLIGHTSHADOWMAPSHADERS_H
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include <GL/gl3w.h>  
#include "ShaderBase.h"
#include "Mesh.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

class PointLightShadowMapShaders : public ShaderBase
{
public:
	PointLightShadowMapShaders();
	PointLightShadowMapShaders(int otherAssetID);
	~PointLightShadowMapShaders();

	void CreateShaderData();

	GLuint PointLightShaderProgram;
	int assetID;

};

#endif // !POINTLIGHTSHADOWMAPSHADERS_H

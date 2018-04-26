#include "ShaderProgramLib.h"

ShaderProgramLib::ShaderProgramLib()
{
}

ShaderProgramLib::~ShaderProgramLib()
{
	for (int i = 0; i < shaders.size(); i++)
	{
		delete shaders[i];
	}
}

void ShaderProgramLib::addGeometryPassShaders()
{
	//add your shader here
	GeometryShaders* newShaderProgram = new GeometryShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addLightpassShaders()
{
	//add your shader here
	LightpassShaders* newShaderProgram = new LightpassShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addShadowMapShaders()
{
	//add your shader here
	ShadowMapShader* newShaderProgram = new ShadowMapShader();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addAnimationShaders()
{
	AnimationShaders* newShaderProgram = new AnimationShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addTerrainShaders()
{
	TerrainShaders* newShaderProgram = new TerrainShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addMainMenuShaders()
{
	MainMenuShader* newShaderProgram = new MainMenuShader();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addUIShaders()
{
	UIShaders* newShaderProgram = new UIShaders();
	shaders.push_back(newShaderProgram);
}

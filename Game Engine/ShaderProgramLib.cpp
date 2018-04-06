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

void ShaderProgramLib::addCubeMapShaders()
{
	//add your shader here
	CubeMapShaders* newShaderProgram = new CubeMapShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addSSAOShaders()
{
	//add your shader here
	SSAOShaders* newShaderProgram = new SSAOShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addBlurShaders()
{
	//add your shader here
	SSAOBlurShaders* newShaderProgram = new SSAOBlurShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addLightpassShaders()
{
	//add your shader here
	LightpassShaders* newShaderProgram = new LightpassShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addGaussianBlurShaders()
{
	//add your shader here
	GaussianBlurShaders* newShaderProgram = new GaussianBlurShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addSkyboxShaders()
{
	//add your shader here
	SkyboxShaders* newShaderProgram = new SkyboxShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addFXAAShaders()
{
	//add your shader here
	FXAAShaders* newShaderProgram = new FXAAShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addShadowMapShaders()
{
	//add your shader here
	ShadowMapShader* newShaderProgram = new ShadowMapShader();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addPointLightShadowMapShaders()
{
	//add your shader here
	PointLightShadowMapShaders* newShaderProgram = new PointLightShadowMapShaders();
	shaders.push_back(newShaderProgram);
}

void ShaderProgramLib::addAnimationShaders()
{
	AnimationShaders* newShaderProgram = new AnimationShaders();
	shaders.push_back(newShaderProgram);
}

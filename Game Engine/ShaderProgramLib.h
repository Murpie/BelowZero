#pragma once
#include<vector>
#include "Shader.h"
#include "GeometryShaders.h"
#include "CubeMapShaders.h"
#include "SSAOShaders.h"
#include "SSAOBlurShaders.h"
#include "LightpassShaders.h"
#include "SkyboxShaders.h"
#include "GaussianBlurShaders.h"
#include "FXAAShaders.h"
#include <string>
#include "Mesh.h"
#include "ShaderBase.h"
#include "ShadowMapShaders.h"

class ShaderProgramLib
{
public:
	ShaderProgramLib();

	~ShaderProgramLib();

	void addGeometryPassShaders();
	void addCubeMapShaders();
	void addSSAOShaders();
	void addBlurShaders();
	void addLightpassShaders();
	void addGaussianBlurShaders();
	void addSkyboxShaders();
	void addFXAAShaders();
	void addShadowMapShaders();

	template <class T>
	T* getShader() {
		for (unsigned int i = 0; i < shaders.size(); i++)
		{
			if (dynamic_cast<T*>(shaders[i]) != nullptr)
			{
				T *test = dynamic_cast<T*>(shaders[i]);
				return test;
			}
		}

		return nullptr;
	}

private:
	std::vector<ShaderBase*> shaders;

};
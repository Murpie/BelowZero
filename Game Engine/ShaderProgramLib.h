#pragma once
#include<vector>
#include "Shader.h"
#include "GeometryShaders.h"
#include "CubeMapShaders.h"
#include "LightpassShaders.h"
#include "SkyboxShaders.h"
#include "AnimationShaders.h"
#include <string>
#include "Mesh.h"
#include "ShaderBase.h"
#include "ShadowMapShaders.h"
#include "PointLightShadowMapShaders.h"
#include "UIShaders.h"

class ShaderProgramLib
{
public:
	ShaderProgramLib();
	~ShaderProgramLib();

	void addGeometryPassShaders();
	void addCubeMapShaders();
	void addLightpassShaders();
	void addSkyboxShaders();
	void addShadowMapShaders();
	void addPointLightShadowMapShaders();
	void addAnimationShaders();
	void addUIShaders();

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
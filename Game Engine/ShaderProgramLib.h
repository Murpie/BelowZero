#pragma once
#include<vector>
#include "Shader.h"
#include "GeometryShaders.h"
#include "LightpassShaders.h"
#include "AnimationShaders.h"
#include <string>
#include "Mesh.h"
#include "ShaderBase.h"
#include "ShadowMapShaders.h"
#include "UIShaders.h"
#include "TerrainShaders.h"
#include "VFXShaders.h"

class ShaderProgramLib
{
public:
	ShaderProgramLib();
	~ShaderProgramLib();

	void addGeometryPassShaders();
	void addLightpassShaders();
	void addShadowMapShaders();
	void addAnimationShaders();
	void addUIShaders();
	void addVFXShaders();
	void addTerrainShaders();

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
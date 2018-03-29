#include "MaterialLib.h"

MaterialLib::MaterialLib()
{
	materials = std::vector<Material>();
}

MaterialLib::~MaterialLib()
{
}

void MaterialLib::addMaterial(GLuint otherShaderProgram)
{
	Material newMat = Material(otherShaderProgram);
	materials.push_back(newMat);
}

Material * MaterialLib::getMaterial(int key)
{
	return &materials.at(key);
}

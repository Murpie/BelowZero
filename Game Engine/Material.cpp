#include "Material.h"

Material::Material():Component()
{
	assetName = "Material";
	Component::id = ComponentType::ID::MATERIAL;
}

Material::Material(GLuint otherShaderProgram)
{
	Component::id = ComponentType::ID::MATERIAL;
	assetName = "Material";
	geometryShaderProgram = otherShaderProgram;
}

Material::~Material()
{
}

void Material::update(float deltaTime)
{
}

void Material::processEvents(GLFWwindow * window, float deltaTime)
{
}

void Material::bindTextures()
{

	//Albedo Map
	if (albedo.size() > 0)
	{
		glUniform1i(glGetUniformLocation(geometryShaderProgram, "gAlbedoIn"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo[0]);
	}
	//Normal Map
	if (normal.size() > 0)
	{
		glUniform1i(glGetUniformLocation(geometryShaderProgram, "gNormalIn"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal[0]);
	}
	//Specular
	if (specular.size() > 0)
	{
		glUniform1i(glGetUniformLocation(geometryShaderProgram, "gSpecularIn"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, specular[0]);
	}
	//Metallic
	if (metallic.size() > 0)
	{
		glUniform1i(glGetUniformLocation(geometryShaderProgram, "gMetallicIn"), 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, metallic[0]);
	}
	//AO
	if (ao.size() > 0)
	{
		glUniform1i(glGetUniformLocation(geometryShaderProgram, "gAOIn"), 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao[0]);
	}
}

void Material::bindFoundTextures()
{
	glUniformMatrix3fv(glGetUniformLocation(geometryShaderProgram, "mtlInfo"), 1, GL_FALSE, glm::value_ptr(mtlInfo));
	glUniform1i(glGetUniformLocation(geometryShaderProgram, "foundAlbedo"), foundAlbedo[0]);
	glUniform1i(glGetUniformLocation(geometryShaderProgram, "foundNormal"), foundNormal[0]);
	glUniform1i(glGetUniformLocation(geometryShaderProgram, "foundSpecular"), foundSpecular[0]);
	glUniform1i(glGetUniformLocation(geometryShaderProgram, "foundMetallic"), foundMetallic[0]);
	glUniform1i(glGetUniformLocation(geometryShaderProgram, "foundAO"), foundAO[0]);
}

void Material::addAlbedo(int otherTexture)
{
	albedo.push_back(otherTexture);
}

void Material::addNormal(int otherTexture)
{
	normal.push_back(otherTexture);
}

void Material::addSpecular(int otherTexture)
{
	specular.push_back(otherTexture);
}

void Material::addMetallic(int otherTexture)
{
	metallic.push_back(otherTexture);
}

void Material::addAO(int otherTexture)
{
	ao.push_back(otherTexture);
}

void Material::addFoundAlbedo(int foundTexture)
{
	foundAlbedo.push_back(foundTexture);
}

void Material::addFoundNormal(int foundTexture)
{
	foundNormal.push_back(foundTexture);
}

void Material::addFoundSpecular(int foundTexture)
{
	foundSpecular.push_back(foundTexture);
}

void Material::addFoundMetallic(int foundTexture)
{
	foundMetallic.push_back(foundTexture);
}

void Material::addFoundAO(int foundTexture)
{
	foundAO.push_back(foundTexture);
}

void Material::addMtlInfo(glm::mat3 mtlInfo)
{
	this->mtlInfo = mtlInfo;
}

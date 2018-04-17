#pragma once
#include <GL/gl3w.h>  
#include "stb_image.h"
#include <string>
#include<iostream>
#include"Component.h"
#include <vector>
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include "glm\glm\glm.hpp"

class Material:public Component
{
public:
	Material();
	Material(GLuint otherShaderProgram);
	~Material();

	std::vector<GLuint> albedo;
	std::vector<GLuint> normal;
	std::vector<GLuint> specular;
	std::vector<GLuint> metallic;
	std::vector<GLuint> ao;
	std::vector<GLuint> foundAlbedo;
	std::vector<GLuint> foundNormal;
	std::vector<GLuint> foundSpecular;
	std::vector<GLuint> foundMetallic;
	std::vector<GLuint> foundAO;
	glm::mat3 mtlInfo;

	GLuint geometryShaderProgram;

	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);
	
	void bindTextures();
	void bindFoundTextures();
	void addAlbedo(int otherTexture);
	void addNormal(int otherTexture);
	void addSpecular(int otherTexture);
	void addMetallic(int otherTexture);
	void addAO(int otherTexture);
	void addFoundAlbedo(int foundTexture);
	void addFoundNormal(int foundTexture);
	void addFoundSpecular(int foundTexture);
	void addFoundMetallic(int foundTexture);
	void addFoundAO(int foundTexture);
	void addMtlInfo(glm::mat3 mtlInfo);

private:
	int width, height, nrChannels = 0;
};


#pragma once
#include <GL/gl3w.h>  
#include "stb_image.h"
#include <string>
#include<iostream>

class Texture
{
public:
	Texture();
	Texture(std::string filePath);
	~Texture();

	void CreateTextureData(std::string filePath);
	GLuint gTexture;
private:
	int width, height, nrChannels = 0;

};


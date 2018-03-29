#include "TextureLib.h"

TextureLib::TextureLib()
{
}
TextureLib::~TextureLib()
{
}

void TextureLib::addAlbedo(std::string filePath)
{
	Texture newMat = Texture(filePath);
	albedo.push_back(newMat);
}

void TextureLib::addNormal(std::string filePath)
{
	Texture newMat = Texture(filePath);
	normal.push_back(newMat);
}

void TextureLib::addSpecular(std::string filePath)
{
	Texture newMat = Texture(filePath);
	specular.push_back(newMat);
}

void TextureLib::addMetallic(std::string filePath)
{
	Texture newMat = Texture(filePath);
	metallic.push_back(newMat);
}

void TextureLib::addAO(std::string filePath)
{
	Texture newMat = Texture(filePath);
	ao.push_back(newMat);
}

const Texture * TextureLib::getAlbedo(int key)
{
	return &albedo.at(key);
}

const Texture * TextureLib::getNormal(int key)
{
	return &normal.at(key);
}

const Texture * TextureLib::getSpecular(int key)
{
	return &specular.at(key);
}

const Texture * TextureLib::getMetallic(int key)
{
	return &metallic.at(key);
}

const Texture * TextureLib::getAO(int key)
{
	return &ao.at(key);
}
#pragma once
#include<vector>
#include"Texture.h"
#include<string>

class TextureLib
{
public:
	TextureLib();
	~TextureLib();

	//void addTexture(std::string filePath);
	void addAlbedo(std::string filePath);
	void addNormal(std::string filePath);
	void addSpecular(std::string filePath);
	void addMetallic(std::string filePath);
	void addAO(std::string filePath);

	const Texture* getAlbedo(int key);
	const Texture* getNormal(int key);
	const Texture* getSpecular(int key);
	const Texture* getMetallic(int key);
	const Texture* getAO(int key);

	std::vector<Texture> albedo;
	std::vector<Texture> normal;
	std::vector<Texture> specular;
	std::vector<Texture> metallic;
	std::vector<Texture> ao;

private:
	
};
#pragma once

#include <vector>
#include <string>

#include "Component.h"
#include "Texture.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

struct TerrainVertex
{
	float x;
	float y; 
	float z;
	float r;
	float g;
	float b;
	float u;
	float v;
	//glm::vec3 vertPos;
	//glm::vec3 vertNorm;
	//glm::vec2 UVs;
};

class Terrain : public Component
{
private:
	float MAX_HEIGHT = 40;
	float MIN_HEIGHT = -40;
	int sizePerSide = 32;
	float MAX_PIXEL_COLOR = 256 * 256 * 256;
	Texture HeightMap;
	Texture AlbedoMap;


	int xLength;
	int xHeight;
	int offset = 0;
	

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;
	GLuint EBO;
	GLuint FBO;
	GLuint PBO;

	bool foundAlbedo = true;
	bool foundNormal = false;
	bool foundSpecular = false;
	bool foundMetallic = false;
	bool foundAO = false;



public:
	Terrain();
	Terrain(const std::string & height, const std::string & color, GLuint shader);
	~Terrain();

	std::vector<TerrainVertex> terrainVertices;

	std::vector<float> heights;
	std::vector<unsigned short> indices;

	short *heightMapIndexData;

	int vertexCount;
	int indexCount;
	void setupVertexData();
	void setupBuffers(GLint gShaderProgram);
	//void calculateNormals(int triangleIndex);
	//float getHeight(int x, int z);
	float getHeightRGB(int x, int y);

	void loadHeighMap(const std::string & heightMap);

	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);
	GLuint getVBO();
	GLuint getEBO();
	GLuint getVAO();
	void bindVertexArray();
	void bindTextures(GLuint shader);
};
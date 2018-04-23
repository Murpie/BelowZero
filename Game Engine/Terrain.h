#pragma once

#include <vector>
#include <string>
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/glm.hpp"
#include "Component.h"
#include "Texture.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

struct TerrainVertex
{
	float x, y, z;
	float r, g, b;
	float u, v;
	//glm::vec3 vertPos;
	//glm::vec3 vertNorm;
	//glm::vec2 UVs;
};

class Terrain : public Component
{
private:
	float MAX_HEIGHT = 10;
	float MIN_HEIGHT = -3;
	int sizePerSide = 32;
	float MAX_PIXEL_COLOR = 256 * 256 * 256;
	Texture HeightMap;
	Texture AlbedoMap;

	int offset = 8;

	int Length = 40;
	int Height = 40;

	float **Heights;

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

	float xOffset;
	float zOffset;


public:
	Terrain();
	Terrain(const std::string & height, const std::string & color, GLuint shader);
	~Terrain();



	std::vector<TerrainVertex> terrainVertices;

	std::vector<unsigned int> indices;

	short *heightMapIndexData;

	int vertexCount;
	int indexCount;
	void setupVertexData();
	void setupBuffers(GLint gShaderProgram);
	//void calculateNormals(int triangleIndex);
	float getHeight(int x, int z);
	float getHeightRGB(int x, int y);

	void loadHeighMap(const std::string & heightMap);

	void update(float deltaTime, float seconds);
	void processEvents(GLFWwindow *window, float deltaTime);
	GLuint getVBO();
	GLuint getEBO();
	GLuint getVAO();
	void bindVertexArray();
	void bindTextures(GLuint shader);

	float calculateY(float x, float z);


	float leftVertex(int x, int z);
	float rightVertex(int x, int z);
	float frontVertex(int x, int z);
	float behindVertex(int x, int z);
	float frontLeftVertex(int x, int z);
	float frontRightVertex(int x, int z);

	float distanceBetweenVertices();
	float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
};
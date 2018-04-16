#pragma once

#include <vector>
#include <string>

#include "Texture.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

struct TerrainVertex
{
	glm::vec3 vertPos;
	glm::vec3 vertNorm;
	glm::ivec2 UVs;
};

class Terrain
{
private:
	float MAX_HEIGHT = 40;
	float MIN_HEIGHT = -40;

	float MAX_PIXEL_COLOR = 256 * 256 * 256;
	Texture HeightMap;
	Texture AlbedoMap;

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;
	GLuint EBO;
	GLuint PBO;
	GLuint FBO;
	std::vector<TerrainVertex> terrainVertices;

	std::vector<glm::vec3> Normals;
	std::vector<float> heights;
	std::vector<glm::vec3> VertexPos;
	std::vector<glm::vec2> Uvs;
	std::vector<glm::ivec3> indices;

	int vertexCount;

public:
	Terrain();
	Terrain(const std::string & height, const std::string & color);
	~Terrain();

	void setupVertexData();
	void setupBuffers(GLint gShaderProgram);
	void calculateNormals(int triangleIndex);
	float getHeight(int x, int z);

	void loadHeighMap(const std::string & heightMap);

};
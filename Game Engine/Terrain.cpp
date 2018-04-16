#include "Terrain.h"

Terrain::Terrain()
{
}

Terrain::Terrain(const std::string & height, const std::string & color)
{
	HeightMap.CreateTextureData(height);
	AlbedoMap.CreateTextureData(color);


}

Terrain::~Terrain()
{
}

void Terrain::setupVertexData()
{
	glBindTexture(GL_TEXTURE_2D, HeightMap.gTexture);
	glGenFramebuffers(1, &PBO);
	glBindFramebuffer(GL_FRAMEBUFFER, PBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HeightMap.gTexture, 0);

	vertexCount = HeightMap.width * HeightMap.height;
	std::vector<float>temp;

	GLubyte* pixels = (GLubyte*)malloc(HeightMap.width * HeightMap.height * sizeof(GLubyte) * 4);

	glm::vec3 test;
	float test2;
	for (int i = 0; i < HeightMap.width; i++)
	{
		temp.clear();
		for (int j = 0; j < HeightMap.height; j++)
		{
			TerrainVertex temp;

			glReadPixels(i, j, HeightMap.width, HeightMap.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			temp.UVs = glm::vec2((float)i / (float)HeightMap.width, (float)j / (float)HeightMap.height);
			float h = (float)(int)pixels[0] / 255;
			temp.vertPos = glm::vec3(i - (int)(HeightMap.height / 2), h, j - (int)(HeightMap.width / 2));

			terrainVertices.push_back(temp);
		}
	}


	//for (int i = 0; i < VertexPos.size() - HeightMap.width -1; i++)
	//{
	//	indices.push_back(glm::vec3(i, i+1, i+HeightMap.width));
	//	indices.push_back(glm::vec3(i+1, i + HeightMap.width, i + HeightMap.width));
	//}
	//for (int i = 0; i < indices.size(); i++)
	//{
	//	glm::vec3 v1 = terrainVertices[indices[i].x].vertPos;
	//	glm::vec3 v2 = terrainVertices[indices[i].y].vertPos;
	//	glm::vec3 v3 = terrainVertices[indices[i].z].vertPos;
	//
	//	glm::vec3 ab = v2 - v1;
	//	glm::vec3 ac = v3 - v1;
	//	glm::vec3 normal = glm::normalize(glm::cross(ab, ac));
	//	terrainVertices[indices[i].x].vertNorm = normal;
	//	terrainVertices[indices[i].y].vertNorm = normal;
	//	terrainVertices[indices[i].z].vertNorm = normal;
	//}

}

void Terrain::setupBuffers(GLint gShaderProgram)
{
	//vao
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	//vbo
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(TerrainVertex), &terrainVertices[0], GL_STATIC_DRAW);

	//gElementBuffer
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
	GLuint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");

	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(
		vertexPos,				// location in shader
		3,						// how many elements of type (see next argument)
		GL_FLOAT,				// type of each element
		GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
		sizeof(terrainVertices), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);

	// repeat the process for the second attribute.
	// query which "slot" corresponds to the input vertex_color in the Vertex Shader 
	GLuint vertexColor = glGetAttribLocation(gShaderProgram, "vertex_normal");
	glVertexAttribPointer(
		vertexColor,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(terrainVertices), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 3)	// note, the first color starts after the first vertex.
	);

	// repeat the process for the third attribute.
	// query which "slot" corresponds to the input uv coord in the Vertex Shader 
	GLuint uvPos = glGetAttribLocation(gShaderProgram, "uv_coord");
	glVertexAttribPointer(
		uvPos,
		2,
		GL_FLOAT,
		GL_FALSE, sizeof(terrainVertices), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 6)	// note, the first uv starts after the first color.
	);

}

//float Terrain::getHeight(int x, int z)
//{
//	if (x < 0 || x >= HeightMap.height || z<0 || z>HeightMap.height)
//		return 0.0;
//
//	//glm::vec3 height = HeightMap.getRGB(x, z);
//	float done = glm::mix(height.x, height.y, height.z);
//}

void Terrain::loadHeighMap(const std::string & heightMap)
{
	HeightMap.CreateTextureData(heightMap);
}

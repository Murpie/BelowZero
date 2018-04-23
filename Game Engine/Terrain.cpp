#include "Terrain.h"

Terrain::Terrain()
{
}

Terrain::Terrain(const std::string & height, const std::string & color, GLuint shader)
{
	Component::id = ComponentType::TERRAIN;
	this->HeightMap.CreateTextureData(height);
	this->AlbedoMap.CreateTextureData(color);
	Length = sizePerSide;
	Height = sizePerSide;


	this->setupVertexData();


	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		TerrainVertex temp;
	//		temp.x = (float)i * 400;
	//		temp.y = 1.2f;
	//		temp.z = (float)j * 400;
	//		temp.r = 0.0;
	//		temp.g = 0.0;
	//		temp.b = 0.0;
	//		temp.u = 0.0;
	//		temp.v = 0.0;
	//
	//		terrainVertices.push_back(temp);
	//	}
	//}
	//
	//for (int y = 0; y < 4 - 1; y++)
	//{
	//	if (y > 0)
	//		indices.push_back((short)(y * 4));
	//
	//	for (int x = 0; x < 4; x++)
	//	{
	//		indices.push_back((short)((y * 4) + x));
	//		indices.push_back((short)((y + 1) * 4) + x);
	//	}
	//	if (y < 4 - 2)
	//	{
	//		indices.push_back((short)((y + 1) * 4) + 4 -1);
	//	}
	//
	//
	//
	//}

	this->setupBuffers(shader);
}

Terrain::~Terrain()
{
	for (int i = 0; i < this->Length; i++)
		delete Heights[i];

	delete[] Heights;
}

void Terrain::setupVertexData()
{
	glBindTexture(GL_TEXTURE_2D, this->HeightMap.gTexture);
	glGenFramebuffers(1, &this->PBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->PBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->HeightMap.gTexture, 0);

	//this->vertexCount = this->HeightMap.width * this->HeightMap.height;
	this->vertexCount = Length * Height;

	GLubyte* pixels = (GLubyte*)malloc(this->HeightMap.width * this->HeightMap.height * sizeof(GLubyte) * 4);

	glm::vec3 test;
	float test2;

	int heightTemp = (int)(HeightMap.height / Height);
	int lengthTemp = (int)(HeightMap.width / Length);

	this->zOffset = Height * offset;
	this->xOffset = Length * offset;

	Heights = new float*[Height];

	for (int i = 0; i < this->Height; i++)
	{
		Heights[i] = new float[Length];
		for (int j = 0; j < this->Length; j++)
		{
			TerrainVertex temp;

			glReadPixels((i*lengthTemp), (j*heightTemp), this->HeightMap.width, this->HeightMap.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);



			temp.x = (float)j * offset;//(float)(j - (Height / 2)) * offset;

			float tempY = ((float)(int)pixels[0] / 255) * offset;
			tempY -= offset;

			temp.y = tempY;
			temp.z = ((float)i * offset); //(float)(i - (Height / 2)) * offset;

			temp.r = 0.0f;
			temp.g = 0.0f;
			temp.b = 0.0f;

			temp.u = (float)j / (float)(this->Length - 1);
			temp.v = -(float)i / (float)(this->Height - 1);

			this->terrainVertices.push_back(temp);

			Heights[i][j] = tempY;
		}
	}

	free(pixels);

	//for (int i = 0; i < this->HeightMap.height; i++)
	//{
	//	for (int j = 0; j < this->HeightMap.width; j++)
	//	{
	//		TerrainVertex temp;

	//		glReadPixels(i, j, this->HeightMap.width, this->HeightMap.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	//		temp.u = (float)j / (float)this->HeightMap.width;
	//		temp.v = (float)i / (float)this->HeightMap.height;
	//		temp.x = (j - (HeightMap.width / 2)) * 400;
	//		temp.y = (float)(int)pixels[0] / 255;
	//		temp.z = (i - (HeightMap.height / 2)) * 400;
	//		temp.r = 0.0f;
	//		temp.g = 0.0f;
	//		temp.b = 0.0f;

	//		this->terrainVertices.push_back(temp);


	//	}
	//}


	for (int y = 0; y < this->Height - 1; y++)
	{
		if (y > 0)
			indices.push_back((short)(y * Height));

		for (int x = 0; x < this->Length; x++)
		{
			indices.push_back((short)((y * Height) + x));
			indices.push_back((short)((y + 1) * Height) + x);
		}
		if (y < HeightMap.width - 2)
		{
			indices.push_back((short)((y + 1) * Height) + Length - 1);
		}



	}
	indexCount = indices.size();

	//for (unsigned int i = 0; i < this->terrainVertices.size() - this->Length -1; i++)
	//{
	//	this->indices.push_back((unsigned int)i);
	//	this->indices.push_back((unsigned int)i + 1);
	//	this->indices.push_back((unsigned int)i + Length);
	//	this->indices.push_back((unsigned int)i + 1);
	//	this->indices.push_back((unsigned int)i + Length);
	//	this->indices.push_back((unsigned int)i + Length + 1);
	//}
	//
	//for (int i = 0; i < this->indices.size(); i += 3)
	//{
	//	if (i >= 4500)
	//		int temp = 0;
	//
	//	glm::vec3 v1 = glm::vec3(this->terrainVertices[indices[i]].x, this->terrainVertices[indices[i]].y, this->terrainVertices[indices[i]].z);
	//	glm::vec3 v2 = glm::vec3(this->terrainVertices[indices[i+1]].x, this->terrainVertices[indices[i + 1]].y, this->terrainVertices[indices[i + 1]].z);
	//	glm::vec3 v3 = glm::vec3(this->terrainVertices[indices[i+2]].x, this->terrainVertices[indices[i + 2]].y, this->terrainVertices[indices[i + 2]].z);
	//
	//	glm::vec3 ab = v1 - v2;
	//	glm::vec3 ac = v1 - v3;
	//
	//	glm::vec3 normal = glm::normalize(glm::cross(ac, ab));
	//
	//
	//	this->terrainVertices[(int)this->indices[i]].r += normal.x;
	//	this->terrainVertices[(int)this->indices[i]].g += normal.y;
	//	this->terrainVertices[(int)this->indices[i]].b += normal.z;
	//
	//	this->terrainVertices[(int)this->indices[i + 1]].r += normal.x;
	//	this->terrainVertices[(int)this->indices[i + 1]].g += normal.y;
	//	this->terrainVertices[(int)this->indices[i + 1]].b += normal.z;
	//
	//	this->terrainVertices[(int)this->indices[i + 2]].r += normal.x;
	//	this->terrainVertices[(int)this->indices[i + 2]].g += normal.y;
	//	this->terrainVertices[(int)this->indices[i + 2]].b += normal.z;
	//
	//} 
	for (int i = 0; i < this->indices.size()-2; i += 2)
	{

		glm::vec3 v1 = glm::vec3(this->terrainVertices[indices[i]].x, this->terrainVertices[indices[i]].y, this->terrainVertices[indices[i]].z);
		glm::vec3 v2 = glm::vec3(this->terrainVertices[indices[i + 1]].x, this->terrainVertices[indices[i + 1]].y, this->terrainVertices[indices[i + 1]].z);
		glm::vec3 v3 = glm::vec3(this->terrainVertices[indices[i + 2]].x, this->terrainVertices[indices[i + 2]].y, this->terrainVertices[indices[i + 2]].z);

		glm::vec3 ab = v1 - v2;
		glm::vec3 ac = v1 - v3;

		glm::vec3 normal = glm::normalize(glm::cross(ac, ab));


		this->terrainVertices[(int)this->indices[i]].r += normal.x;
		this->terrainVertices[(int)this->indices[i]].g += normal.y;
		this->terrainVertices[(int)this->indices[i]].b += normal.z;

		this->terrainVertices[(int)this->indices[i + 1]].r += normal.x;
		this->terrainVertices[(int)this->indices[i + 1]].g += normal.y;
		this->terrainVertices[(int)this->indices[i + 1]].b += normal.z;

		this->terrainVertices[(int)this->indices[i + 2]].r += normal.x;
		this->terrainVertices[(int)this->indices[i + 2]].g += normal.y;
		this->terrainVertices[(int)this->indices[i + 2]].b += normal.z;

	}

}

void Terrain::setupBuffers(GLint gShaderProgram)
{



	//vao
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//vbo
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(TerrainVertex), &terrainVertices[0], GL_STATIC_DRAW);

	//gElementBuffer
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

	// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
	GLuint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");

	if (vertexPos == -1) {
		OutputDebugStringA("Error, cannot find 'vertex_position' attribute in Vertex shader\n");
	}
	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(
		vertexPos,				// location in shader
		3,						// how many elements of type (see next argument)
		GL_FLOAT,				// type of each element
		GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
		sizeof(TerrainVertex), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);
	//
	// repeat the process for the second attribute.
	// query which "slot" corresponds to the input vertex_color in the Vertex Shader 
	GLuint vertexColor = glGetAttribLocation(gShaderProgram, "vertex_normal");

	if (vertexColor == -1) {
		OutputDebugStringA("Error, cannot find 'vertex_normal' attribute in Vertex shader\n");
		return;
	}

	glVertexAttribPointer(
		vertexColor,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(TerrainVertex), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 3)	// note, the first color starts after the first vertex.
	);

	// repeat the process for the third attribute.
	// query which "slot" corresponds to the input uv coord in the Vertex Shader 
	GLuint uvPos = glGetAttribLocation(gShaderProgram, "uv_coord");

	if (uvPos == -1) {
		OutputDebugStringA("Error, cannot find 'uv_coord' attribute in Vertex shader\n");
		return;
	}

	glVertexAttribPointer(
		uvPos,
		2,
		GL_FLOAT,
		GL_FALSE, sizeof(TerrainVertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 6)	// note, the first uv starts after the first color.
	);

}

float Terrain::getHeight(int x, int z)
{
	if (x < 0 || x >= Height - 1 || z<0 || z >= Length - 1)
		return 0.0;

	return Heights[x][z];


	//return terrainVertices[((x+1) * (z+1)) - 1].y;

	//glm::vec3 height = HeightMap.getRGB(x, z);

}

float Terrain::getHeightRGB(int x, int y)
{
	return 0.0f;
}

void Terrain::loadHeighMap(const std::string & heightMap)
{
	this->HeightMap.CreateTextureData(heightMap);
}

void Terrain::update(float deltaTime, float seconds)
{
}

void Terrain::processEvents(GLFWwindow * window, float deltaTime)
{
}
GLuint Terrain::getVBO()
{
	return this->VBO;
}

GLuint Terrain::getEBO()

{
	return this->EBO;
}

GLuint Terrain::getVAO()
{
	return this->VAO;
}

void Terrain::bindVertexArray()
{
	//glBindBuffer(GL_ARRAY_BUFFER, this->VAO);


	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBindVertexArray(this->VAO);

}

void Terrain::bindTextures(GLuint shader)
{
	glUniform1i(glGetUniformLocation(shader, "gAlbedoIn"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->AlbedoMap.gTexture);


	glUniform1i(glGetUniformLocation(shader, "foundAlbedo"), this->foundAlbedo);


}

float Terrain::calculateY(float x, float z)
{
	float terrainX = x;
	float terrainZ = z;




	float gridSquareSize = offset/* * ((Length * Height) - 1)*/;//((float)terrainVertices.size() - 1);

	int gridX = (int)glm::floor(terrainX / gridSquareSize);
	int gridZ = (int)glm::floor(terrainZ / gridSquareSize);

	if (gridX >= terrainVertices.size() - 1 || gridZ >= terrainVertices.size() - 1 || gridX < 0 || gridZ < 0)
		return 0;

	float xCoord = ((int)terrainX % (int)gridSquareSize) / gridSquareSize;
	float zCoord = ((int)terrainZ % (int)gridSquareSize) / gridSquareSize;
	float answer;
	if (xCoord <= (1 - zCoord))
	{
		answer = barryCentric(
			glm::vec3(0, Heights[gridZ][gridX], 0),
			glm::vec3(1, Heights[gridZ + 1][gridX], 0),
			glm::vec3(0, Heights[gridZ][gridX + 1], 1),
			glm::vec2(xCoord, zCoord));
	}
	else
	{
		answer = barryCentric(
			glm::vec3(1, Heights[gridZ + 1][gridX], 0),
			glm::vec3(1, Heights[gridZ + 1][gridX + 1], 1),
			glm::vec3(0, Heights[gridZ][gridX + 1], 1),
			glm::vec2(xCoord, zCoord));
	}
	answer += 2;
	return answer;

}

float Terrain::leftVertex(int x, int z)
{
	if ((x - 1) * z < 0 || (x - 1) * z > terrainVertices.size())
		return 0.0;

	return this->terrainVertices[(x - 1) * z].y;

}

float Terrain::rightVertex(int x, int z)
{
	if ((x + 1) * z < 0 || (x + 1) * z > terrainVertices.size())
		return 0.0;

	return this->terrainVertices[(x + 1) * z].y;
}

float Terrain::frontVertex(int x, int z)
{
	if (x * (z + 1) < 0 || x * (z + 1) > terrainVertices.size())
		return 0.0;

	return this->terrainVertices[x * (z + 1)].y;
}

float Terrain::behindVertex(int x, int z)
{
	if (x * (z - 1) < 0 || x * (z - 1) > terrainVertices.size())
		return 0.0;

	return this->terrainVertices[x * (z - 1)].y;
}

float Terrain::distanceBetweenVertices()
{
	return offset;
}

float Terrain::barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
{
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}
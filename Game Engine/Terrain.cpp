#include "Terrain.h"

Terrain::Terrain()
{
}

Terrain::Terrain(const std::string & height, const std::string & color, GLuint shader)
{
	Component::id = ComponentType::TERRAIN;
	this->HeightMap.CreateTextureData(height);
	this->AlbedoMap.CreateTextureData(color);
	xLength = sizePerSide;
	xHeight = sizePerSide;

	this->VAO = 0;
	this->VBO = 0;
	this->IBO = 0;
	this->EBO = 0;
	this->FBO = 0;
	this->PBO = 0;

	this->setupVertexData();
	this->setupBuffers(shader);


}

Terrain::~Terrain()
{
	
}

void Terrain::setupVertexData()
{
	glBindTexture(GL_TEXTURE_2D, this->HeightMap.gTexture);
	glGenFramebuffers(1, &this->PBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->PBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->HeightMap.gTexture, 0);

	this->vertexCount = this->HeightMap.width * this->HeightMap.height;


	GLubyte* pixels = (GLubyte*)malloc(this->HeightMap.width * this->HeightMap.height * sizeof(GLubyte) * 4);



	glm::vec3 test;
	float test2;
	for (int i = 0; i < this->HeightMap.height; i++)
	{
		for (int j = 0; j < this->HeightMap.width; j++)
		{
			TerrainVertex temp;

			glReadPixels(i, j, this->HeightMap.width, this->HeightMap.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			temp.u = (float)j / (float)this->HeightMap.width;
			temp.v = (float)i / (float)this->HeightMap.height;
			temp.x = j - (int)(HeightMap.width / 2);
			temp.y = (float)(int)pixels[0] / 255;
			temp.z = i - (int)(HeightMap.height / 2);
			temp.r = 0.0f;
			temp.g = 0.0f;
			temp.b = 0.0f;

			this->terrainVertices.push_back(temp);


		}
	}
	int nrofstripsrequired = this->HeightMap.height - 1;
	int nrofDegensRequired = 2 * (nrofstripsrequired - 1);
	int VerticesPerStrip = 2 * this->HeightMap.width;
	const int indexes = (VerticesPerStrip * nrofstripsrequired) + nrofDegensRequired;
	heightMapIndexData = new short[indexes];

	for (int y = 0; y < this->HeightMap.height - 1; y++)
	{
		if (y > 0)
		indices.push_back((short)(y * HeightMap.height));
	
		for (int x = 0; x < this->HeightMap.width; x++)
		{
			indices.push_back((short)((y * HeightMap.height) + x));
			indices.push_back((short)((y + 1) * HeightMap.height) + x);
		}
		if (y < HeightMap.width - 2)
		{
			indices.push_back((short)((y + 1) * HeightMap.height));
		}
	
	
	
	}
	indexCount = indices.size();

	//for (int i = 0; i < this->terrainVertices.size() - this->HeightMap.width -1; i++)
	//{
	//
	//
	//
	//	indices.push_back(i);
	//	indices.push_back(i + 1);
	//	indices.push_back(i + this->HeightMap.width);
	//	indices.push_back(i + 1);
	//	indices.push_back(i + this->HeightMap.width);
	//	indices.push_back(i + this->HeightMap.width + 1);
	//}
	//for (int i = 0; i < this->indices.size(); i+= 3)
	//{
	//	glm::vec3 v1 = glm::vec3(this->terrainVertices[indices[i]].x, this->terrainVertices[indices[i]].y, this->terrainVertices[indices[i]].z);
	//	glm::vec3 v2 = glm::vec3(this->terrainVertices[indices[i+1]].x, this->terrainVertices[indices[i + 1]].y, this->terrainVertices[indices[i + 1]].z);
	//	glm::vec3 v3 = glm::vec3(this->terrainVertices[indices[i+2]].x, this->terrainVertices[indices[i + 2]].y, this->terrainVertices[indices[i + 2]].z);
	//
	//	glm::vec3 ab = v2 - v1;
	//	glm::vec3 ac = v3 - v1;
	//	glm::vec3 normal = glm::normalize(glm::cross(ab, ac));
	//	this->terrainVertices[this->indices[i]].r += normal.x;
	//	this->terrainVertices[this->indices[i]].g += normal.y;
	//	this->terrainVertices[this->indices[i]].b += normal.z;
	//	this->terrainVertices[this->indices[i + 1]].r += normal.x;
	//	this->terrainVertices[this->indices[i + 1]].g += normal.y;
	//	this->terrainVertices[this->indices[i + 1]].b += normal.z;
	//	this->terrainVertices[this->indices[i + 2]].r += normal.x;
	//	this->terrainVertices[this->indices[i + 2]].g += normal.y;
	//	this->terrainVertices[this->indices[i + 2]].b += normal.z;
	//}

}

void Terrain::setupBuffers(GLint gShaderProgram)
{
	//vao
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//vbo
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->terrainVertices.size() * sizeof(TerrainVertex), &this->terrainVertices[0], GL_STATIC_DRAW);

	//gElementBuffer
	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexCount * sizeof(short), &this->indices[0], GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

	// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
	GLuint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");

	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(
		vertexPos,				// location in shader
		3,						// how many elements of type (see next argument)
		GL_FLOAT,				// type of each element
		GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
		sizeof(this->terrainVertices), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);
	//
	// repeat the process for the second attribute.
	// query which "slot" corresponds to the input vertex_color in the Vertex Shader 
	GLuint vertexColor = glGetAttribLocation(gShaderProgram, "vertex_normal");
	glVertexAttribPointer(
		vertexColor,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(this->terrainVertices), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 3)	// note, the first color starts after the first vertex.
	);
	
	// repeat the process for the third attribute.
	// query which "slot" corresponds to the input uv coord in the Vertex Shader 
	GLuint uvPos = glGetAttribLocation(gShaderProgram, "uv_coord");
	glVertexAttribPointer(
		uvPos,
		2,
		GL_FLOAT,
		GL_FALSE, sizeof(this->terrainVertices), // distance between two uv coord 
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
	
	glBindVertexArray(this->VBO); 
}

void Terrain::bindTextures(GLuint shader)
{
	glUniform1i(glGetUniformLocation(shader, "gAlbedoIn"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->AlbedoMap.gTexture);


	glUniform1i(glGetUniformLocation(shader, "foundAlbedo"), this->foundAlbedo);
	glUniform1i(glGetUniformLocation(shader, "foundNormal"), this->foundNormal);
	glUniform1i(glGetUniformLocation(shader, "foundSpecular"), this->foundSpecular);
	glUniform1i(glGetUniformLocation(shader, "foundMetallic"), this->foundMetallic);
	glUniform1i(glGetUniformLocation(shader, "foundAO"), this->foundAO);

	
}


#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(std::string filePath, GLuint gShaderProgram)
{
	//vertexCount = 0;
	CreateMeshData(filePath, gShaderProgram);
}

Mesh::~Mesh()
{
}

void Mesh::CreateMeshData(std::string filePath, GLuint gShaderProgram)
{
	LeapImporter importer;

	leapMesh = importer.getMesh(filePath.c_str());



	//vao
	glGenVertexArrays(1, &gVertexAttribute);
	glBindVertexArray(gVertexAttribute);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//vbo
	glGenBuffers(1, &gVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, leapMesh->getVertexCount() * sizeof(VertexInformation), &leapMesh->vertices[0], GL_STATIC_DRAW);

	GLuint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");

	if (vertexPos == -1) {
		OutputDebugStringA("Error, cannot find 'vertex_position' attribute in Vertex shader\n");
		return;
	}

	glVertexAttribPointer(
		vertexPos,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(VertexInformation),
		BUFFER_OFFSET(0)
	);

	GLuint vertexNormals = glGetAttribLocation(gShaderProgram, "vertex_normal");
	glVertexAttribPointer(
		vertexNormals,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(VertexInformation),
		BUFFER_OFFSET(sizeof(float) * 3)
	);

	GLuint uvPos = glGetAttribLocation(gShaderProgram, "uv_coord");
	glVertexAttribPointer(
		uvPos,
		2,
		GL_FLOAT,
		GL_FALSE, sizeof(VertexInformation),
		BUFFER_OFFSET(sizeof(float) * 6)
	);

	//importer.deleteObject(leapMesh);
}

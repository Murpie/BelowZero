#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(std::string filePath, GLuint gShaderProgram, GLuint meshType)
{
	vertexCount = 0;
	this->meshType = meshType;
	CreateMeshData(filePath, gShaderProgram);
}

Mesh::~Mesh()
{
}

void Mesh::CreateMeshData(std::string filePath, GLuint gShaderProgram)
{
	std::ifstream infile.read((char*))


	std::vector<int> finalIndex;
	int uniqueFoundIndices = 0;
	Vertex newMesh;
	std::vector<Vertex> newVertex;
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices, temp_normals;
	std::vector<glm::vec2> temp_uvs;
	glm::vec3 vertex;
	glm::vec3 uv;
	glm::vec3 normals;

	// Read from OBJ file
	const char *filePathChar = filePath.c_str();
	FILE *file = fopen(filePathChar, "r");
	if (file == NULL)
	{
		printf("An error occured while trying to open the mesh file!\n");
	}
	else
	{
		while (true)
		{
			char lineHeader[128];
			int word = fscanf(file, "%s", lineHeader);
			if (word == EOF) //EOF = End Of Line.
			{
				break;
			}
			else if (strcmp(lineHeader, "v") == 0)
			{
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				fscanf(file, "%f %f %f\n", &normals.x, &normals.y, &normals.z);
				temp_normals.push_back(normals);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				GLuint vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				//Kolla så rätt antal doubles finns på varje "f" rad
				if (matches != 9)
				{
					printf("File cannot be read at ""f""");
				}
				else
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);

					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);

					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
			}

		}

		for (int i = 0; i < vertexIndices.size(); i++)
		{
			//Skapa en vertis
			glm::vec3 vertex = temp_vertices[vertexIndices[i] - 1];
			newMesh.x = vertex.x;
			newMesh.y = vertex.y;
			newMesh.z = vertex.z;
			glm::vec2 uv = temp_uvs[uvIndices[i] - 1];
			newMesh.u = uv.x;
			newMesh.v = -uv.y;

			glm::vec3 normal = temp_normals[normalIndices[i] - 1];
			newMesh.nx = normal.x;
			newMesh.ny = normal.y;
			newMesh.nz = normal.z;

			//loopa igenom alla vertiser som redan finns, push_backa vertisen bara om det är en ny vertis
			bool okToPusBack = true;
			int index = -1;
			for (int i = 0; i < Vertices.size(); i++)
			{
				if (Vertices[i].x == newMesh.x && Vertices[i].y == newMesh.y && Vertices[i].z == newMesh.z && Vertices[i].u == newMesh.u && Vertices[i].v == newMesh.v && Vertices[i].nx == newMesh.nx && Vertices[i].ny == newMesh.ny && Vertices[i].nz == newMesh.nz) {
					okToPusBack = false;
					index = i;

					break;
				}
			}

			//är det en ny vertis så push_backar vi det indexet och inkrementerar uniqueFoundIndices
			if (okToPusBack) {
				finalIndex.push_back(uniqueFoundIndices);
				uniqueFoundIndices++;
				Vertices.push_back(newMesh);
			}
			//är det en gammal vertis så tar vi indexvärdet som vi fick ur loopen
			else
			{
				finalIndex.push_back(index);

			}
		}

	}

	//inverterar vectorn för att windingorder ska bli rätt, annars är normalerna åt fel håll och sen sätter vi polycount för att kunna hämta det i Render()
	std::reverse(finalIndex.begin(), finalIndex.end());
	vertexCount = finalIndex.size();

	for (unsigned int t = 0; t < finalIndex.size() - 1; t += 3)
	{
		//Calculate Tangents
		int vert1 = finalIndex[t];
		int vert2 = finalIndex[t + 1];
		int vert3 = finalIndex[t + 2];

		glm::vec3 pos1 = glm::vec3(Vertices.at(vert1).x, Vertices.at(vert1).y, Vertices.at(vert1).z);
		glm::vec3 pos2 = glm::vec3(Vertices.at(vert2).x, Vertices.at(vert2).y, Vertices.at(vert2).z);
		glm::vec3 pos3 = glm::vec3(Vertices.at(vert3).x, Vertices.at(vert3).y, Vertices.at(vert3).z);

		glm::vec2 uv1 = glm::vec2(Vertices.at(vert1).u, Vertices.at(vert1).v);
		glm::vec2 uv2 = glm::vec2(Vertices.at(vert2).u, Vertices.at(vert2).v);
		glm::vec2 uv3 = glm::vec2(Vertices.at(vert3).u, Vertices.at(vert3).v);

		glm::vec3 n = glm::vec3(Vertices.at(vert1).nx, Vertices.at(vert1).ny, Vertices.at(vert1).nz);

		glm::vec3 tangent = glm::vec3(0, 0, 0);
		glm::vec3 bitangent = glm::vec3(0, 0, 0);

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		Vertices.at(vert1).tx = tangent.x;
		Vertices.at(vert1).ty = tangent.y;
		Vertices.at(vert1).tz = tangent.z;
		Vertices.at(vert2).tx = tangent.x;
		Vertices.at(vert2).ty = tangent.y;
		Vertices.at(vert2).tz = tangent.z;
		Vertices.at(vert3).tx = tangent.x;
		Vertices.at(vert3).ty = tangent.y;
		Vertices.at(vert3).tz = tangent.z;

		Vertices.at(vert1).bx = bitangent.x;
		Vertices.at(vert1).by = bitangent.y;
		Vertices.at(vert1).bz = bitangent.z;
		Vertices.at(vert2).bx = bitangent.x;
		Vertices.at(vert2).by = bitangent.y;
		Vertices.at(vert2).bz = bitangent.z;
		Vertices.at(vert3).bx = bitangent.x;
		Vertices.at(vert3).by = bitangent.y;
		Vertices.at(vert3).bz = bitangent.z;
	}

	//vao
	glGenVertexArrays(1, &gVertexAttribute);
	glBindVertexArray(gVertexAttribute);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	//vbo
	glGenBuffers(1, &gVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

	//gElementBuffer
	unsigned int gElementBuffer;
	glGenBuffers(1, &gElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, finalIndex.size() * sizeof(unsigned int), &finalIndex[0], GL_STATIC_DRAW);

	// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
	GLuint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");
	// if this returns -1, it means there is a problem, and the program will likely crash.
	// examples: the name is different or missing in the shader

	if (vertexPos == -1) {
		OutputDebugStringA("Error, cannot find 'vertex_position' attribute in Vertex shader\n");
		return;
	}

	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(
		vertexPos,				// location in shader
		3,						// how many elements of type (see next argument)
		GL_FLOAT,				// type of each element
		GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
		sizeof(Vertex), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);

	// repeat the process for the second attribute.
	// query which "slot" corresponds to the input vertex_color in the Vertex Shader 
	GLuint vertexColor = glGetAttribLocation(gShaderProgram, "vertex_normal");
	glVertexAttribPointer(
		vertexColor,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(Vertex), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 3)	// note, the first color starts after the first vertex.
	);

	// repeat the process for the third attribute.
	// query which "slot" corresponds to the input uv coord in the Vertex Shader 
	GLuint uvPos = glGetAttribLocation(gShaderProgram, "uv_coord");
	glVertexAttribPointer(
		uvPos,
		2,
		GL_FLOAT,
		GL_FALSE, sizeof(Vertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 6)	// note, the first uv starts after the first color.
	);

	GLuint tangent = glGetAttribLocation(gShaderProgram, "tangent");
	glVertexAttribPointer(
		tangent,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(Vertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 8)	// note, the first uv starts after the first color.
	);

	GLuint bitangent = glGetAttribLocation(gShaderProgram, "bitangent");
	glVertexAttribPointer(
		bitangent,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(Vertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 11)	// note, the first uv starts after the first color.
	);
}

#include "AnimatedMesh.h"


AnimatedMesh::AnimatedMesh()
{
}
AnimatedMesh::AnimatedMesh(std::string filePath, GLuint gShaderProgram)
{

}

AnimatedMesh::~AnimatedMesh()
{
}

void AnimatedMesh::Update(GLuint shader)
{
	for (int i = 0; i < Joints.size(); i++)
	{
		UpdateKeyFrameTransform(time, i);
	}
	calculateFinalMatrices();

	for (int i = 0; i < FinalMatrices.size(); i++)
	{
		std::string name = "Bones[" + std::to_string(i) + "]";
		GLint BoneMatrices = glGetUniformLocation(shader, name.c_str());
		glUniformMatrix4fv(BoneMatrices, 1, GL_FALSE, glm::value_ptr(FinalMatrices[i]));
	}


}
unsigned int AnimatedMesh::FindRotation(float time, int index)
{
	for (int i = 0; i < Joints[index].rotVector.size(); i++)
	{
		if (time < (float)Joints[index].rotTimes[i + 1])
			return i;
	}
	return 0;
}
unsigned int AnimatedMesh::FindPosition(float time, int index)
{
	for (int i = 0; i < Joints[index].posVector.size(); i++)
	{
		if (time < (float)Joints[index].posTimes[i + 1])
			return i;
	}
	return 0;
}
unsigned int AnimatedMesh::FindScale(float time, int index)
{
	for (int i = 0; i < Joints[index].scaleVector.size(); i++)
	{
		if (time < (float)Joints[index].scaleTimes[i + 1])
			return i;
	}
	return 0;
}

glm::vec3 AnimatedMesh::CalcInterpolatedPosition(float time, int index)
{
	if (Joints[index].posVector.size() == 1)
	{
		glm::vec3 val(Joints[index].posVector[0]);
		return val;
	}

	unsigned int PositionIndex = FindPosition(time, index);
	unsigned int nextPosition = PositionIndex + 1;

	double deltaTime = Joints[index].posTimes[nextPosition] - Joints[index].posTimes[PositionIndex];
	if (deltaTime < 0.001)
		deltaTime = 0.1;

	float factor = (time - Joints[index].posTimes[PositionIndex]) / deltaTime;
	if (factor < 0)
		factor = 0;
	if (factor > 1)
		factor = 1;

	glm::vec3 p1(Joints[index].posVector[PositionIndex]);
	glm::vec3 p2(Joints[index].posVector[nextPosition]);
	glm::vec3 val = glm::mix(p1, p2, val);

	return val;
}

glm::quat AnimatedMesh::CalcInterpolatedRotation(float time, int index)
{
	if (Joints[index].rotVector.size() == 1)
	{
		glm::quat val(Joints[index].rotVector[0]);
		return val;
	}

	unsigned int RotationIndex = FindRotation(time, index);
	unsigned int nextRotation = RotationIndex + 1;

	double deltaTime = Joints[index].rotTimes[nextRotation] - Joints[index].rotTimes[RotationIndex];
	if (deltaTime < 0.001)
		deltaTime = 0.1;

	float factor = (time - Joints[index].rotTimes[RotationIndex]) / deltaTime;
	if (factor < 0)
		factor = 0;
	if (factor > 1.0)
		factor = 1.0;

	glm::quat r1(Joints[index].rotVector[RotationIndex]);
	glm::quat r2(Joints[index].rotVector[nextRotation]);
	glm::quat val = glm::slerp(r1, r2, factor);

	return val;
}
glm::vec3 AnimatedMesh::CalcInterpolatedScale(float time, int index)
{
	if (Joints[index].scaleVector.size() == 1)
	{
		glm::vec3 val(Joints[index].scaleVector[0]);
		return val;
	}

	unsigned int ScaleIndex = FindScale(time, index);
	unsigned int nextScale = ScaleIndex + 1;

	double deltaTime = Joints[index].scaleTimes[nextScale] - Joints[index].scaleTimes[ScaleIndex];
	if (deltaTime < 0.001)
		deltaTime = 0.1;

	float factor = (time - Joints[index].scaleTimes[ScaleIndex]) / deltaTime;
	if (factor < 0)
		factor = 0;
	if (factor > 1)
		factor = 1;

	glm::vec3 p1(Joints[index].scaleVector[ScaleIndex]);
	glm::vec3 p2(Joints[index].scaleVector[nextScale]);
	glm::vec3 val = glm::mix(p1, p2, val);

	return val;
}



void AnimatedMesh::UpdateKeyFrameTransform(float time, int index)
{
	if (Joints[index].posVector.size() != 0)
		Joints[index].pos = CalcInterpolatedPosition(time, index);
	if (Joints[index].rotVector.size() != 0)
		Joints[index].rot = CalcInterpolatedRotation(time, index);
	if (Joints[index].scaleVector.size() != 0)
		Joints[index].scale = CalcInterpolatedScale(time, index);

	glm::mat4 mat = glm::mat4(1);

	mat *= glm::translate(glm::mat4(1), Joints[index].pos);
	mat *= glm::mat4_cast(Joints[index].rot);
	mat *= glm::scale(glm::mat4(1), Joints[index].scale);

	Joints[index].transformMatrix = mat;
}

void AnimatedMesh::CreateMeshData(std::string filePath, GLuint gShaderProgram)
{
	//Get Information ------==========||==========-------





















	std::vector<int> finalIndex;
	int uniqueFoundIndices = 0;
	SkelletonVertex newMesh;
	std::vector<SkelletonVertex> newVertex;
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices, temp_normals;
	std::vector<glm::vec2> temp_uvs;
	glm::vec3 vertex;
	glm::vec3 uv;
	glm::vec3 normals;
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
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(SkelletonVertex), &Vertices[0], GL_STATIC_DRAW);

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
		sizeof(SkelletonVertex), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);

	// repeat the process for the second attribute.
	// query which "slot" corresponds to the input vertex_color in the Vertex Shader 
	GLuint vertexColor = glGetAttribLocation(gShaderProgram, "vertex_normal");
	glVertexAttribPointer(
		vertexColor,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(SkelletonVertex), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 3)	// note, the first color starts after the first vertex.
	);

	// repeat the process for the third attribute.
	// query which "slot" corresponds to the input uv coord in the Vertex Shader 
	GLuint uvPos = glGetAttribLocation(gShaderProgram, "uv_coord");
	glVertexAttribPointer(
		uvPos,
		2,
		GL_FLOAT,
		GL_FALSE, sizeof(SkelletonVertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 6)	// note, the first uv starts after the first color.
	);

	GLuint tangent = glGetAttribLocation(gShaderProgram, "tangent");
	glVertexAttribPointer(
		tangent,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(SkelletonVertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 8)	// note, the first uv starts after the first color.
	);

	GLuint bitangent = glGetAttribLocation(gShaderProgram, "bitangent");
	glVertexAttribPointer(
		bitangent,
		3,
		GL_FLOAT,
		GL_FALSE, sizeof(SkelletonVertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 11)	// note, the first uv starts after the first color.
	);

	GLuint weights = glGetAttribLocation(gShaderProgram, "weights");
	glVertexAttribPointer(
		bitangent,
		4,
		GL_FLOAT,
		GL_FALSE, sizeof(SkelletonVertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 14)	// note, the first uv starts after the first color.
	);

	GLuint boneID = glGetAttribLocation(gShaderProgram, "boneID");
	glVertexAttribPointer(
		bitangent,
		4,
		GL_FLOAT,
		GL_FALSE, sizeof(SkelletonVertex), // distance between two uv coord 
		BUFFER_OFFSET(sizeof(float) * 18)	// note, the first uv starts after the first color.
	);
}

glm::mat4 AnimatedMesh::getParentTransform(Joint joint)
{
	std::vector<glm::mat4> mats;
	int parentIndex = joint.parentIndex;
	Joint b = joint;
	if (parentIndex != -1)
	{
		b = Joints[parentIndex];
		parentIndex = b.parentIndex;
	}
	while (parentIndex != -1)
	{
		glm::mat4 tmp_Mat = b.transformMatrix;
		mats.push_back(tmp_Mat);

		b = Joints[b.parentIndex];
		parentIndex = b.parentIndex;
	}
	glm::mat4 concatenated_transforms = glm::mat4(1);
	for (int i = mats.size() - 1; i >= 0; i--)
		concatenated_transforms *= mats.at(i);

	return concatenated_transforms;

}

void AnimatedMesh::calculateFinalMatrices()
{
	FinalMatrices.clear();
	for (int i = 0; i < Joints.size(); i++)
	{
		if (i > Joints.size() - 1)
			FinalMatrices.push_back(glm::mat4(1));
		else
		{
			glm::mat4 ct = getParentTransform(Joints[i]) * Joints[i].transformMatrix;
			FinalMatrices.push_back(glm::inverse(Joints[0].transformMatrix) * ct * Joints[i].offsetMatrix);
		}
	}
}

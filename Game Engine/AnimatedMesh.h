#define BUFFER_OFFSET(i) ((char *)nullptr + (i))


#include <windows.h>
#include <string>
#include <GL/gl3w.h>  
#include "SkelletonVertex.h"
#include "Animation.h"
#include "Joint.h"
#include <vector>
#include <iostream>
#include "glm/glm/glm.hpp"
#include <algorithm>    // std::unique, std::distance

class AnimatedMesh
{
public:
	AnimatedMesh();
	AnimatedMesh(std::string filePath, GLuint gShaderProgram);
	~AnimatedMesh();
	float time = 0.0;

	void Update(GLuint shader);

	void UpdateKeyFrameTransform(float time, int index);

	unsigned int FindRotation(float time, int index);
	unsigned int FindPosition(float time, int index);
	unsigned int FindScale(float time, int index);

	glm::vec3 CalcInterpolatedPosition(float time, int index);
	glm::quat CalcInterpolatedRotation(float time, int index);
	glm::vec3 CalcInterpolatedScale(float time, int index);

	void CreateMeshData(std::string filePath, GLuint gShaderProgram);
	glm::mat4 getParentTransform(Joint joint);
	void calculateFinalMatrices();

	std::vector<glm::mat4>FinalMatrices;
	std::vector<SkelletonVertex> Vertices;
	std::vector<Animation> Animations;
	std::vector<Joint> Joints;
	std::vector<unsigned int> indices;
	int vertexCount;
	int jointCount;
	int animationCount;
	GLuint gVertexBuffer;
	GLuint gVertexAttribute;
	GLuint gElementBuffer;
};

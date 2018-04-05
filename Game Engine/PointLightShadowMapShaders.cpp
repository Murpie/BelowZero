#include "PointLightShadowMapShaders.h"

PointLightShadowMapShaders::PointLightShadowMapShaders()
{
	CreateShaderData();
}

PointLightShadowMapShaders::PointLightShadowMapShaders(int otherAssetID)
{
	assetID = otherAssetID;
	CreateShaderData();
}

PointLightShadowMapShaders::~PointLightShadowMapShaders()
{
}

void PointLightShadowMapShaders::CreateShaderData()
{
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	std::ifstream shaderFile("PointLightShadowMapVS.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	const char* shaderTextPtr = shaderText.c_str();

	glShaderSource(vs, 1, &shaderTextPtr, nullptr);

	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		glGetShaderInfoLog(vs, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	shaderFile.open("PointLightShadowMapGS.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(gs, 1, &shaderTextPtr, nullptr);
	glCompileShader(gs);
	compileResult = GL_FALSE;
	glGetShaderiv(gs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) 
	{
		memset(buff, 0, 1024);
		glGetShaderInfoLog(gs, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("PointLightShadowMapFS.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) 
	{
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	PointLightShaderProgram = glCreateProgram();
	glAttachShader(PointLightShaderProgram, vs);
	glAttachShader(PointLightShaderProgram, gs);
	glAttachShader(PointLightShaderProgram, fs);
	glLinkProgram(PointLightShaderProgram);

	compileResult = GL_FALSE;
	glGetProgramiv(PointLightShaderProgram, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) 
	{
		memset(buff, 0, 1024);
		glGetProgramInfoLog(PointLightShaderProgram, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	glDetachShader(PointLightShaderProgram, vs);
	glDetachShader(PointLightShaderProgram, gs);
	glDetachShader(PointLightShaderProgram, fs);
	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
}

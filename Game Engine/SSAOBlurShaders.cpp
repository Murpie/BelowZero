#include "SSAOBlurShaders.h"

SSAOBlurShaders::SSAOBlurShaders()
{
	CreateShaderData();
}

SSAOBlurShaders::SSAOBlurShaders(int otherAssetID)
{
	assetID = otherAssetID;
	CreateShaderData();
}

SSAOBlurShaders::~SSAOBlurShaders()
{
}

void SSAOBlurShaders::CreateShaderData()
{
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	std::ifstream shaderFile("ssaoBlurVS.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	const char* shaderTextPtr = shaderText.c_str();

	glShaderSource(vs, 1, &shaderTextPtr, nullptr);

	glCompileShader(vs);

	// check for compilation error
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		glGetShaderInfoLog(vs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("ssaoBlurFS.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}


	//link shader program (connect vs and ps)
	ssaoBlurShaderProgram = glCreateProgram();
	glAttachShader(ssaoBlurShaderProgram, fs);
	glAttachShader(ssaoBlurShaderProgram, vs);
	glLinkProgram(ssaoBlurShaderProgram);

	// check once more, if the Vertex Shader and the Fragment Shader can be used
	// together
	compileResult = GL_FALSE;
	glGetProgramiv(ssaoBlurShaderProgram, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetProgramInfoLog(ssaoBlurShaderProgram, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}
	// in any case (compile sucess or not), we only want to keep the 
	// Program around, not the shaders.
	glDetachShader(ssaoBlurShaderProgram, vs);
	glDetachShader(ssaoBlurShaderProgram, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <string>
#include <glad/glad.h>

class Shader
{
public:
	Shader();

	void attachShader(const char* fileName, GLenum shaderType);

	void link();

	void use();

	int getProgram()
	{
		return _programID;
	}

	int getUniformLocation(const char* varName)
	{
		return glGetUniformLocation(_programID, varName);
	}

private:
	unsigned int _programID;

	std::string getShaderFromFile(const char* fileName);

	void checkShaderCompileErrors(GLuint shader, const std::string& type);
};

#endif
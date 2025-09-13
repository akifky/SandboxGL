#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <iostream>

Shader::Shader() {
	_programID = glCreateProgram();
}

void Shader::link() {
	glLinkProgram(_programID);
}

void Shader::use() {
	glUseProgram(_programID);
}

void Shader::attachShader(const char* fileName, GLenum shaderType) {
	
	unsigned int shaderID = glCreateShader(shaderType);
	std::string shaderSource_string = getShaderFromFile(fileName);
	const char* shaderSource_char = shaderSource_string.c_str();

	glShaderSource(shaderID,1, &shaderSource_char, 0);
	glCompileShader(shaderID);
    checkShaderCompileErrors(shaderID, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
	glAttachShader(_programID,shaderID);
	glDeleteShader(shaderID);
}

std::string Shader::getShaderFromFile(const char* fileName) {
    std::ifstream file(fileName);
    std::string data;

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open shader file: " << fileName << "\n";
        return data;
    }

    char readChar;
    while ((readChar = file.get()) != EOF) {
        data += readChar;
    }
    file.close();
    return data;
}

void Shader::checkShaderCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Error: Shader Compile Error (" << type << "): " << infoLog << "\n";
    }
}
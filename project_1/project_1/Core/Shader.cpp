#include <glad/glad.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Shader::GLCacheActiveShader = -1;

// Constructor generates the shader on the fly
Shader::Shader(const char* vertexPath, const char* fragmentPath, std::string name) {
	this->name = name;
	this->blendMode = Opaque;
	//1. Retrieve the vertex/fragment source code from filePath
	//TODO - understand fstreams, strings and all these things in C++
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. Compile shaders
	unsigned int vertex, fragment;
	// vertex
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
Shader::Shader(std::string vertexPath, std::string fragmentPath, std::string name):
	Shader(vertexPath.c_str(), fragmentPath.c_str(), name){
}
Shader::Shader(const std::string shaderName, std::string name)
	: Shader(("Assets/Shaders/" + shaderName + ".vert").c_str(),
		("Assets/Shaders/" + shaderName + ".frag").c_str(),
		name
	) {
}
// This activates the shader for following renders
void Shader::use() {
	if (ID != GLCacheActiveShader) {
		glUseProgram(ID);
		GLCacheActiveShader = ID;
	}
}
//This destroys the OpenGL shader program
void Shader::cleanup() {
	if (ID != 0) {
		glDeleteProgram(ID);
		ID = 0;
	}
}
// Uniform setting functions
void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVector3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setVector3(const std::string& name, glm::vec3 v) const
{
	this->setVector3(name, v.x, v.y, v.z);
}
void Shader::setVector2(const std::string& name, float x, float y) const{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::setMat4(const std::string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
// Utility function for checking shader compilation/linking errors
void Shader::checkCompileErrors(unsigned int shader, std::string type) {
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- ------------------" << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- ------------------" << std::endl;
		}
	}
};
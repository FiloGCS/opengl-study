#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {

	//Moving forward, for vertex attributes we're going to try to adhere to:
	// 0 - Position
	// 1 - Color
	// 2 - UVs
	// 3 - Normals

public:
	// the shader program OpenGL ID
	unsigned int ID;

	// This constructor generates the shader program from two shader paths
	Shader(const char* vertexPath, const char* fragmentPath);
	// This constructor assumes that both shaders are in "Shaders/" with the same name
	Shader(std::string shaderName);
	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVector3(const std::string& name, float x, float y, float z) const;
	void setMat4(const std::string& name, glm::mat4 value) const;

private:
	void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
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
	std::string name;

	//---CONSTRUCTORS
	// This constructor generates the shader program from two shader paths
	Shader(const char* vertexPath, const char* fragmentPath, std::string name);
	// This constructor assumes that both shaders are in "Shaders/" with the same name
	Shader(std::string shaderName, std::string name);
	//Disable copy constructor and disable assignment operator
	//	This prevents shallow copying, which can mean multiple Shader objects share ID.
	//	That would be bad news.
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	//For both cases, implement moving instead
	Shader(Shader&& other) noexcept : ID(other.ID) {
		other.ID = 0;
	}
	Shader& operator=(Shader&& other) noexcept {
		if (this != &other) {
			glDeleteProgram(ID);
			ID = other.ID;
			other.ID = 0;
		}
		return *this;
	}
	///In simpler terms, lines 1 and 2 are saying "you can't copy this class",
	// while line 3 is saying "but you can move it efficiently".
	// The reason for this design is to prevent accidental copying of
	// Shader objects(which could lead to multiple objects thinking they own the 
	// same OpenGL shader), while still allowing them to be efficiently transferred
	// when needed(like when returning a Shader from a function).

	//---DESTRUCTORS
	~Shader() {
		//The destructor deletes the OpenGL shader program.
		glDeleteProgram(ID);
	}

	//TODO Missing documentation
	void use();
	void cleanup();

	// Uniform setting functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVector3(const std::string& name, float x, float y, float z) const;
	void setVector3(const std::string& name, glm::vec3 v) const;
	void setVector2(const std::string& name, float x, float y) const;
	void setMat4(const std::string& name, glm::mat4 value) const;


private:
	void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
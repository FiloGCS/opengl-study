#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture2D.h"

class RenderObject {

public:

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int BoxVBO;
	Shader shader;

	RenderObject();
	void Render(glm::mat4 projection, glm::mat4 view);
	glm::mat4 getModelMatrix();

};


#endif

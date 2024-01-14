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

class Entity {

public:

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	unsigned int VAO;
	unsigned int VBO;
	Shader shader;

	Entity();
	void Update(double time);
	void Render(glm::mat4 projection, glm::mat4 view);
	glm::mat4 getModelMatrix();

	void setRotation(const glm::vec3& eulerAngles);
	void setRotation(const glm::quat& quaternion);
	void setPosition(const glm::vec3& position);
	void setScale(const glm::vec3& scale);


};


#endif

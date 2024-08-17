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
#include "Model.h"

class Entity {

public:

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Model model;
	Shader shader;

	Entity();
	void Start();
	void Update();
	void Render(glm::mat4 projection, glm::mat4 view);
	void Render(glm::mat4 projection, glm::mat4 view, Shader* customShader);
	glm::mat4 getModelMatrix();

	void setRotation(const glm::vec3& eulerAngles);
	void setRotation(const glm::quat& quaternion);
	void setPosition(const glm::vec3& position);
	void setScale(const glm::vec3& scale);

	void setShader(Shader newShader);


};


#endif

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
	unsigned int ID;
	static unsigned int nextID;
	string name;

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Model* model;
	Shader* shader;
	bool isVisible;

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

	void setShader(Shader* newShader);
	static bool compareEntityByZ(const Entity& a, const Entity& b);
	static bool compareEntityByShaderID(const Entity& a, const Entity& b);
	static bool compareDistanceToPoint(const Entity& a, const Entity& b, glm::vec3 p);

private:
	glm::vec3 rotSpeed;

};


#endif

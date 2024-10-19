#include "Entity.h"
#include <GLFW/glfw3.h>
Entity::Entity(){
	//Default transform values
	position = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);
	rotation = glm::quat();

	isVisible = true;

	//Model needs to be assigned outside
	//Shader needs to be assigned outside
}

void Entity::Start()
{
	
}

void Entity::Update() {
	float time = glfwGetTime();
	glm::vec3 rotSpeed = glm::vec3(0.0f, 45.0f, 0.0f);
	rotation = glm::quat(glm::radians(time * rotSpeed));
	//TODO HACK this is just to emulate Update computation
	//int n = 0;
	//for (int i = 0; i < 9990000;i++) {
	//	n++;
	//}
}

void Entity::Render(glm::mat4 projection, glm::mat4 view){
	shader->use();
	//TODO - set all the uniforms like in Program.cpp
	shader->setInt("u_texture1", 0);
	shader->setInt("u_texture2", 1);
	shader->setMat4("model", getModelMatrix());
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	model->Draw(*shader); //TODO is copying shader?

}

void Entity::Render(glm::mat4 projection, glm::mat4 view, Shader* customShader)
{
	customShader->use();
	//TODO - set all the uniforms like in Program.cpp
	customShader->setInt("u_texture1", 0);
	customShader->setInt("u_texture2", 1);
	customShader->setMat4("model", getModelMatrix());
	customShader->setMat4("view", view);
	customShader->setMat4("projection", projection);

	model->Draw(*customShader); //TODO is copying shader?
}

glm::mat4 Entity::getModelMatrix(){
	glm::mat4 m = glm::mat4(1.0f);
	m = glm::translate(m, position);
	m = m * glm::mat4_cast(rotation);
	m = glm::scale(m, scale);
	return m;
}

void Entity::setRotation(const glm::vec3& eulerAngles){
	rotation = glm::quat(eulerAngles);
}

void Entity::setRotation(const glm::quat& quaternion){
	rotation = quaternion;
}

void Entity::setPosition(const glm::vec3& position){
	this->position = position;
}

void Entity::setScale(const glm::vec3& scale){
	this->scale = scale;
}

void Entity::setShader(Shader* newShader) {
	//TODO do I need to cleanup the previous one here?
	shader = newShader;
}

bool Entity::compareEntityByZ(const Entity& a, const Entity& b) {
	return a.position.z < b.position.z;
}
bool compareEntityByShaderID(const Entity& a, const Entity& b) {
	//This should sort the entities by shader blendMode and ID;
	if (a.shader->blendMode != b.shader->blendMode) {
		return a.shader->blendMode < b.shader->blendMode;
	}
	return a.shader->ID < b.shader->ID;
}



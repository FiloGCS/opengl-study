constexpr char DEFAULT_SHADER_PATH[] = "default";
//char DEFAULT_MODEL_PATH[] = "Assets/Models/suzanne/suzanne_smooth.obj";
char DEFAULT_MODEL_PATH[] = "Assets/Models/cube_smooth/cube_corners.obj";

#include "Entity.h"
#include <GLFW/glfw3.h>
Entity::Entity()
	: model(DEFAULT_MODEL_PATH),
	shader(DEFAULT_SHADER_PATH, "Default Shader") {
	//Default transform values
	position = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);
	rotation = glm::quat();

	//Model
	//Shader
	//Already managed via the initializer list
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
	shader.use();
	//TODO - set all the uniforms like in Program.cpp
	shader.setInt("u_texture1", 0);
	shader.setInt("u_texture2", 1);
	shader.setMat4("model", getModelMatrix());
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	model.Draw(shader); //TODO is copying shader?

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

	model.Draw(*customShader); //TODO is copying shader?
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

void Entity::setShader(Shader newShader) {
	//TODO do I need to cleanup the previous one here?
	shader = std::move(newShader);
}




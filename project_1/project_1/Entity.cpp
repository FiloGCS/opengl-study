constexpr char DEFAULT_SHADER_PATH[] = "07_usingProjection";
constexpr char DEFAULT_TEXTURE_PATH[] = "Textures/T_UV_02.jpg";

#include "Entity.h"
//TODO - Do we need a destructor
Entity::Entity() : shader(DEFAULT_SHADER_PATH){
	//Default transform values
	position = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);
	rotation = glm::quat();

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//VBO
	float boxVertices[] = {
	// X, Y, Z, U, V
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
	//AttribPointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//Textures
	stbi_set_flip_vertically_on_load(true);
	Texture2D texture1(DEFAULT_TEXTURE_PATH);
	//Texture2D texture2("Textures/T_Noise_03a.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1.ID);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texture2.ID);
	//Shader
	//Already managed via the initializer list
}

void Entity::Update(double time) {
	/*setRotation(glm::vec3(glm::radians(time*7 + 30.0f), glm::radians(time * 11 + 45.0f), glm::radians(time * 5 + 60.0f)));*/
	setRotation(glm::vec3(glm::radians(time * 5.0f), glm::radians(time * 7.0f), glm::radians(time * 11.0f)));
}

void Entity::Render(glm::mat4 projection, glm::mat4 view){
	shader.use();
	shader.setInt("u_texture1", 0);
	shader.setInt("u_texture2", 1);
	shader.setMat4("model", getModelMatrix());
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36); //TODO set the correct values

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



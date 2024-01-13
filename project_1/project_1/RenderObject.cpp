constexpr char DEFAULT_SHADER[] = "07_usingProjection";

#include "RenderObject.h"

RenderObject::RenderObject() : shader(DEFAULT_SHADER){
	//Default transform values
	position = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);
	rotation = glm::quat();

	//VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
	//VBO
	float boxVertices[] = {
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
	Texture2D texture1("Textures/T_UV_04.jpg");
	Texture2D texture2("Textures/T_Noise_03a.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1.ID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2.ID);
	//Shader
	//Already managed via the initializer list

}

void RenderObject::Update(double time) {
	std::cout << time << std::endl;

	position.y = glm::sin(time) * 0.25f;
	setRotation(glm::vec3(glm::radians(time*7 + 30.0f), glm::radians(time * 11 + 45.0f), glm::radians(time * 5 + 60.0f)));

}

void RenderObject::Render(glm::mat4 projection, glm::mat4 view){
    shader.use();
	shader.setInt("u_texture1", 0);
	shader.setInt("u_texture2", 1);
    shader.setMat4("model", getModelMatrix());
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36); //TODO set the correct values

}

glm::mat4 RenderObject::getModelMatrix(){
    glm::mat4 m = glm::mat4(1.0f);
	m = glm::translate(m, position);
	m = m * glm::mat4_cast(rotation);
    m = glm::scale(m, scale);
    return m;
}

void RenderObject::setRotation(const glm::vec3& eulerAngles){
	rotation = glm::quat(eulerAngles);
}

void RenderObject::setRotation(const glm::quat& quaternion){
	rotation = quaternion;
}

void RenderObject::setPosition(const glm::vec3& position){
	this->position = position;
}

void RenderObject::setScale(const glm::vec3& scale){
	this->scale = scale;
}



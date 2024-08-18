#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{

public:

	Camera();
	Camera(glm::vec3 position);

	glm::vec3 position;
	glm::quat rotation;
	glm::mat4 GetViewMatrix();

};


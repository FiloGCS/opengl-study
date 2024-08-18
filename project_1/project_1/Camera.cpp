#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Camera::Camera(): position(glm::vec3(0,0,0))
{

}

Camera::Camera(glm::vec3 position)
{
	this->position = position;
}

glm::mat4 Camera::GetViewMatrix()
{
	glm::mat4 view = glm::mat4(1);
	view = glm::translate(view, position);
	//view = view * glm::mat4_cast(rotation);
	return view;
}


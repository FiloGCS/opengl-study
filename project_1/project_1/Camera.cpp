#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Camera::Camera(): position(glm::vec3(0,0,0))
{
}

Camera::Camera(glm::vec3 position)
{
	this->position = position;
	this->rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
    isFocused = true;
}

glm::mat4 Camera::GetViewMatrix()
{
    glm::mat4 view = glm::mat4(1.0f);

    // First, apply the inverse of the rotation
    view = glm::mat4_cast(glm::inverse(rotation)) * view;

    // Then, apply the inverse of the translation
    view = glm::translate(view, -position);

    return view;
}


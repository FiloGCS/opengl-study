#pragma once
#include "Light.h"

Light::Light(const glm::vec3& position, const glm::vec3& color, float falloff, float radius, const std::string& name)
    : position(position), color(color), falloff(falloff), radius(radius), name(name) {
}

Light::Light(const glm::vec3& position, const std::string& name)
    : position(position), color(glm::vec3(1.0f, 1.0f, 1.0f)), falloff(1.0f), radius(5.0f), name(name) {
}
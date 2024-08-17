#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class Light
{
    glm::vec3 position;
    glm::vec3 color;
    float falloff;
    float radius;
    std::string name;

public:
    Light(const glm::vec3& position, const glm::vec3& color, float falloff, float radius, const std::string& name);
    Light(const glm::vec3& position, const std::string& name);
};

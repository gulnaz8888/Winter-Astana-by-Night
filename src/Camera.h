#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position{0.f, 6.f, 28.f};
    glm::vec3 target{0.f, 14.f, -60.f};
    glm::vec3 up{0.f, 1.f, 0.f};
    float fovDegrees = 60.f;
    float nearPlane = 0.5f;
    float farPlane = 400.f;

    glm::mat4 view() const;
    glm::mat4 projection(float aspect) const;
};

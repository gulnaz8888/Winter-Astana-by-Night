#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position{0.f, 5.f, 40.f};
    glm::vec3 target{0.f, 20.f, -55.f};
    glm::vec3 up{0.f, 1.f, 0.f};
    float fovDegrees = 60.f;
    float nearPlane = 1.f;
    float farPlane = 500.f;

    glm::mat4 view() const;
    glm::mat4 projection(float aspect) const;
};

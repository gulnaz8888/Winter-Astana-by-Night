
#include "Lights.h"

#include <glm/glm.hpp>
#include <cmath>
#include <string>

void Lights::apply(
    const Shader& shader,
    const glm::vec3& cameraPosition,
    float time
) {
    shader.use();

    // Camera position for Blinn-Phong highlights
    shader.setVec3("uViewPos", cameraPosition);

    // Material shininess
    shader.setFloat("uShininess", 32.0f);

    // LIGHT 1: Static warm street lamp
    glm::vec3 light1Pos(5.0f, 8.0f, 3.0f);
    glm::vec3 light1Color(1.0f, 0.80f, 0.45f);

    shader.setVec3(
        "uLights[0].position", light1Pos
    );
    shader.setVec3(
        "uLights[0].color", light1Color
    );
    shader.setFloat(
        "uLights[0].intensity", 2.0f
    );

    // LIGHT 2: Moving cool blue light
    float radius = 10.0f;

    glm::vec3 light2Pos(
        std::cos(time) * radius,
        12.0f,
        std::sin(time) * radius
    );

    glm::vec3 light2Color(0.40f, 0.65f, 1.0f);

    shader.setVec3(
        "uLights[1].position", light2Pos
    );
    shader.setVec3(
        "uLights[1].color", light2Color
    );
    shader.setFloat(
        "uLights[1].intensity", 2.0f
    );
}

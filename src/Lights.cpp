
#include "Lights.h"

#include <glm/glm.hpp>
#include <cmath>

void Lights::apply(
    const Shader& shader,
    const glm::vec3& cameraPosition,
    float time,
    int sceneIndex,
    bool enabled
) {
    shader.use();

    // Camera position for Blinn-Phong specular highlights
    shader.setVec3("uViewPos", cameraPosition);
    shader.setFloat("uShininess", 32.0f);

    glm::vec3 light1Pos;
    glm::vec3 light2Pos;

    float light1Intensity;
    float light2Intensity;

    if (sceneIndex == 0) {
        // SCENE 1: Winter City

        // Static warm light near the avenue
        light1Pos = glm::vec3(-12.0f, 12.0f, 10.0f);
        light1Intensity = 2.5f;

        // Moving cool blue light near the buildings
        light2Pos = glm::vec3(
            12.0f + std::cos(time * 0.6f) * 10.0f,
            13.0f,
            -5.0f + std::sin(time * 0.6f) * 12.0f
        );
        light2Intensity = 2.5f;

    }
    else {
        // SCENE 2: EXPO

        // Warm light near the Christmas tree
        light1Pos = glm::vec3(-15.0f, 14.0f, -2.0f);
        light1Intensity = 3.0f;

        // Moving blue light around Nur Alem
        light2Pos = glm::vec3(
            6.0f + std::cos(time * 0.5f) * 18.0f,
            35.0f,
            -42.0f + std::sin(time * 0.5f) * 18.0f
        );
        light2Intensity = 3.0f;
    }

    // When lighting is disabled, both point lights are off
    if (!enabled) {
        light1Intensity = 0.0f;
        light2Intensity = 0.0f;
    }

    // LIGHT 1: Warm yellow
    shader.setVec3(
        "uLights[0].position",
        light1Pos
    );
    shader.setVec3(
        "uLights[0].color",
        glm::vec3(1.0f, 0.80f, 0.45f)
    );
    shader.setFloat(
        "uLights[0].intensity",
        light1Intensity
    );

    // LIGHT 2: Cool blue
    shader.setVec3(
        "uLights[1].position",
        light2Pos
    );
    shader.setVec3(
        "uLights[1].color",
        glm::vec3(0.40f, 0.65f, 1.0f)
    );
    shader.setFloat(
        "uLights[1].intensity",
        light2Intensity
    );
}

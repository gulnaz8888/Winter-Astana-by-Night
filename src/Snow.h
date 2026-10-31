#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "Shader.h"

class Snow {
public:
    explicit Snow(int particleCount = 1500);
    ~Snow();

    Snow(const Snow&) = delete;
    Snow& operator=(const Snow&) = delete;

    void update(float deltaTime);

    void draw(
        const Shader& shader,
        const glm::mat4& view,
        const glm::mat4& projection
    );

private:
    struct Particle {
        glm::vec3 position;
        float speed;
    };

    std::vector<Particle> particles;
    std::vector<glm::vec3> positions;

    GLuint VAO = 0;
    GLuint VBO = 0;

    float randomFloat(float minValue, float maxValue);

    void resetParticle(
        Particle& particle,
        bool randomHeight
    );
};
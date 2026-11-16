
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <random>

#include "Shader.h"

class Fireworks {
public:
    Fireworks();
    ~Fireworks();

    Fireworks(const Fireworks&) = delete;
    Fireworks& operator=(const Fireworks&) = delete;

    void update(float deltaTime);

    void draw(
        const Shader& shader,
        const glm::mat4& view,
        const glm::mat4& projection
    );

private:
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 color;

        float life;
        float maxLife;
    };

    std::vector<Particle> particles;
    std::vector<float> vertexData;

    GLuint VAO = 0;
    GLuint VBO = 0;

    std::mt19937 rng{2026};

    float spawnTimer = 0.0f;
    int burstNumber = 0;

    static constexpr int MAX_PARTICLES = 1200;

    float randomFloat(float min, float max);
    void spawnBurst();
};

#include "Snow.h"

#include <cstdlib>

Snow::Snow(int particleCount) {
    particles.resize(particleCount);
    positions.resize(particleCount);

    for (auto& particle : particles) {
        resetParticle(particle, true);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        positions.size() * sizeof(glm::vec3),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        nullptr
    );

    glBindVertexArray(0);
}

Snow::~Snow() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

float Snow::randomFloat(float minValue, float maxValue) {
    float normalized =
        static_cast<float>(std::rand()) /
        static_cast<float>(RAND_MAX);

    return minValue + normalized * (maxValue - minValue);
}

void Snow::resetParticle(Particle& particle, bool randomHeight) {
    particle.position.x = randomFloat(-40.0f, 40.0f);
    particle.position.z = randomFloat(-30.0f, 20.0f);

    if (randomHeight) {
        particle.position.y = randomFloat(5.0f, 100.0f);
    } else {
        particle.position.y = randomFloat(80.0f, 110.0f);
    }

    particle.speed = randomFloat(4.0f, 12.0f);
}

void Snow::update(float deltaTime) {
    for (auto& particle : particles) {
        particle.position.y -= particle.speed * deltaTime;

        particle.position.x += 0.5f * deltaTime;

        if (particle.position.y < 0.0f) {
            resetParticle(particle, false);
        }
    }
}

void Snow::draw(
    const Shader& shader,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    for (std::size_t i = 0; i < particles.size(); ++i) {
        positions[i] = particles[i].position;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        positions.size() * sizeof(glm::vec3),
        positions.data()
    );

    shader.use();
    shader.setMat4("uView", view);
    shader.setMat4("uProj", projection);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);

    glDrawArrays(
        GL_POINTS,
        0,
        static_cast<GLsizei>(positions.size())
    );

    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
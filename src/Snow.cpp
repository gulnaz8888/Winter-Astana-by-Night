
#include "Snow.h"

#include <algorithm>
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Snow::~Snow() {
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }

    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
}

float Snow::randomFloat(
    float minValue,
    float maxValue
) {
    float normalized =
        static_cast<float>(std::rand()) /
        static_cast<float>(RAND_MAX);

    return minValue +
        normalized * (maxValue - minValue);
}

void Snow::setWind(float x, float z) {
    windVelocity.x = x;
    windVelocity.y = 0.f;
    windVelocity.z = z;
}


void Snow::resetParticle(
    Particle& particle,
    bool randomHeight
) {
    // Wider snowfall area across the winter city
    particle.position.x = randomFloat(
        -85.0f, 85.0f
    );

    particle.position.z = randomFloat(
        -105.0f, 65.0f
    );

    if (randomHeight) {
        particle.position.y = randomFloat(
            5.0f, 100.0f
        );
    }
    else {
        particle.position.y = randomFloat(
            80.0f, 110.0f
        );
    }

    particle.speed = randomFloat(
        4.0f, 12.0f
    );
}


void Snow::update(float deltaTime) {
    // Movement remains independent of frame rate.
    // Clamp large time jumps when switching windows.
    deltaTime = std::clamp(
        deltaTime, 0.f, 0.05f
    );

    for (auto& particle : particles) {
        // Snow falls downward.
        particle.position.y -=
            particle.speed * deltaTime;

        // Wind moves snow horizontally.
        particle.position.x +=
            windVelocity.x * deltaTime;

        particle.position.z +=
            windVelocity.z * deltaTime;

        // Wrap particles across the snowy area.
        // This prevents strong wind from emptying the scene.

        if (particle.position.x > 85.0f) {
            particle.position.x -= 170.0f;
        }
        else if (particle.position.x < -85.0f) {
            particle.position.x += 170.0f;
        }

        if (particle.position.z > 65.0f) {
            particle.position.z -= 170.0f;
        }
        else if (particle.position.z < -105.0f) {
            particle.position.z += 170.0f;
        }


        // Recycle particles reaching the ground.
        if (particle.position.y < 0.f) {
            resetParticle(particle, false);
        }
    }
}

void Snow::draw(
    const Shader& shader,
    const glm::mat4& view,
    const glm::mat4& projection,
    int visibleCount
) {
    int total = static_cast<int>(
        particles.size()
    );

    int count = total;

    if (visibleCount >= 0) {
        count = std::min(
            visibleCount,
            total
        );
    }

    if (count <= 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        positions[i] = particles[i].position;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(count) *
            sizeof(glm::vec3),
        positions.data()
    );

    shader.use();
    shader.setMat4("uView", view);
    shader.setMat4("uProj", projection);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    glBindVertexArray(VAO);

    glDrawArrays(
        GL_POINTS,
        0,
        static_cast<GLsizei>(count)
    );

    glBindVertexArray(0);

    glDisable(GL_BLEND);
}

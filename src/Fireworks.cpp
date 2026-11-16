
#include "Fireworks.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr float PI = 3.14159265359f;

}

Fireworks::Fireworks() {
    particles.reserve(MAX_PARTICLES);
    vertexData.reserve(MAX_PARTICLES * 7);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        MAX_PARTICLES * 7 * sizeof(float),
        nullptr,
        GL_DYNAMIC_DRAW
    );


    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        7 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);


    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        7 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);


    glVertexAttribPointer(
        2,
        1,
        GL_FLOAT,
        GL_FALSE,
        7 * sizeof(float),
        (void*)(6 * sizeof(float))
    );

    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    spawnBurst();
}

Fireworks::~Fireworks() {
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
}

float Fireworks::randomFloat(
    float min,
    float max
) {
    std::uniform_real_distribution<float> distribution(
        min, max
    );

    return distribution(rng);
}

void Fireworks::spawnBurst() {


    const glm::vec3 positions[3] = {
        {-68.f, 95.f, -145.f},
        {0.f, 110.f, -165.f},
        {68.f, 98.f, -145.f}
    };


    const glm::vec3 colors[6] = {
        {1.0f, 0.72f, 0.14f},
        {0.28f, 0.75f, 1.0f},
        {1.0f, 0.32f, 0.72f},
        {0.60f, 1.0f, 0.72f},
        {1.0f, 0.92f, 0.62f},
        {0.76f, 0.45f, 1.0f}
    };

    glm::vec3 center =
        positions[burstNumber % 3];

    glm::vec3 mainColor =
        colors[burstNumber % 6];

    ++burstNumber;


    const int sparkCount = 200;

    for (int i = 0; i < sparkCount; ++i) {

        if (
            static_cast<int>(particles.size())
            >= MAX_PARTICLES
        ) {
            break;
        }


        float theta = randomFloat(
            0.f,
            2.f * PI
        );

        float u = randomFloat(
            -1.f,
            1.f
        );

        float horizontal = std::sqrt(
            std::max(0.f, 1.f - u * u)
        );

        glm::vec3 direction(
            horizontal * std::cos(theta),
            u,
            horizontal * std::sin(theta)
        );


        float speed = randomFloat(
            24.f,
            39.f
        );


        float lifetime = randomFloat(
            2.0f,
            2.8f
        );

        Particle spark;

        spark.position = center;
        spark.velocity = direction * speed;

        spark.color = mainColor;

        spark.life = lifetime;
        spark.maxLife = lifetime;


        if (i % 8 == 0) {
            spark.color = {
                1.0f,
                0.96f,
                0.78f
            };
        }


        if (i % 13 == 0) {
            spark.color = {
                1.0f,
                0.82f,
                0.40f
            };
        }

        particles.push_back(spark);
    }
}

void Fireworks::update(float deltaTime) {


    deltaTime = std::clamp(
        deltaTime,
        0.f,
        0.05f
    );

    spawnTimer += deltaTime;


    if (spawnTimer >= 1.3f) {
        spawnTimer -= 1.3f;
        spawnBurst();
    }

    for (auto& p : particles) {

        p.life -= deltaTime;


        p.velocity.y -=
            6.0f * deltaTime;


        float drag = std::exp(
            -0.30f * deltaTime
        );

        p.velocity *= drag;

        p.position +=
            p.velocity * deltaTime;
    }


    particles.erase(
        std::remove_if(
            particles.begin(),
            particles.end(),
            [](const Particle& p) {
                return p.life <= 0.f;
            }
        ),
        particles.end()
    );
}

void Fireworks::draw(
    const Shader& shader,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    if (particles.empty()) {
        return;
    }

    vertexData.clear();

    for (const auto& p : particles) {

        float alpha = std::clamp(
            p.life / p.maxLife,
            0.f,
            1.f
        );

        vertexData.push_back(
            p.position.x
        );

        vertexData.push_back(
            p.position.y
        );

        vertexData.push_back(
            p.position.z
        );

        vertexData.push_back(
            p.color.r
        );

        vertexData.push_back(
            p.color.g
        );

        vertexData.push_back(
            p.color.b
        );

        vertexData.push_back(alpha);
    }

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        vertexData.size() * sizeof(float),
        vertexData.data()
    );

    shader.use();

    shader.setMat4(
        "uView",
        view
    );

    shader.setMat4(
        "uProj",
        projection
    );

    glEnable(
        GL_PROGRAM_POINT_SIZE
    );


    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE
    );


    glDepthMask(GL_FALSE);

    glBindVertexArray(VAO);

    glDrawArrays(
        GL_POINTS,
        0,
        static_cast<GLsizei>(
            particles.size()
        )
    );

    glBindVertexArray(0);


    glDepthMask(GL_TRUE);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    glDisable(GL_BLEND);
}


#include "Primitives.h"
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

namespace {
constexpr float PI = 3.14159265358979f;


void pushVertex(
    std::vector<float>& vertices,
    const glm::vec3& position,
    const glm::vec3& normal,
    float u = 0.f,
    float v = 0.f
) {
    vertices.insert(vertices.end(), {
        position.x, position.y, position.z,
        normal.x, normal.y, normal.z,
        u, v
    });
}


} // end anonymous namespace

namespace Primitives {

Mesh plane() {
    std::vector<float> vertices;
    glm::vec3 normal(0.f, 1.f, 0.f);

    pushVertex(vertices, {-0.5f, 0.f, -0.5f}, normal, 0.f, 0.f);
    pushVertex(vertices, { 0.5f, 0.f, -0.5f}, normal, 1.f, 0.f);
    pushVertex(vertices, { 0.5f, 0.f,  0.5f}, normal, 1.f, 1.f);
    pushVertex(vertices, {-0.5f, 0.f,  0.5f}, normal, 0.f, 1.f);

    std::vector<unsigned> indices = {
        0, 2, 1,
        0, 3, 2
    };

    return Mesh(vertices, indices);
}

Mesh cube() {
    struct Face {
        glm::vec3 normal;
        glm::vec3 u;
        glm::vec3 v;
    };

    const Face faces[6] = {
        {{0,0,1},  {1,0,0},  {0,1,0}},
        {{0,0,-1}, {-1,0,0}, {0,1,0}},
        {{1,0,0},  {0,0,-1}, {0,1,0}},
        {{-1,0,0}, {0,0,1},  {0,1,0}},
        {{0,1,0},  {1,0,0},  {0,0,-1}},
        {{0,-1,0}, {1,0,0},  {0,0,1}}
    };

    std::vector<float> vertices;
    std::vector<unsigned> indices;

    for (const Face& face : faces) {
        glm::vec3 center =
            face.normal * 0.5f +
            glm::vec3(0.f, 0.5f, 0.f);

        glm::vec3 positions[4] = {
            center - face.u * 0.5f - face.v * 0.5f,
            center + face.u * 0.5f - face.v * 0.5f,
            center + face.u * 0.5f + face.v * 0.5f,
            center - face.u * 0.5f + face.v * 0.5f
        };

        unsigned base =
            static_cast<unsigned>(vertices.size() / 8);

        pushVertex(vertices, positions[0], face.normal, 0.f, 0.f);
        pushVertex(vertices, positions[1], face.normal, 1.f, 0.f);
        pushVertex(vertices, positions[2], face.normal, 1.f, 1.f);
        pushVertex(vertices, positions[3], face.normal, 0.f, 1.f);

        indices.insert(indices.end(), {
            base, base + 1, base + 2,
            base, base + 2, base + 3
        });
    }

    return Mesh(vertices, indices);
}

Mesh cylinder(int segments, float topRadius) {
    std::vector<float> vertices;
    std::vector<unsigned> indices;

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.f * PI * i / segments;
        float x = std::cos(angle);
        float z = std::sin(angle);

        glm::vec3 normal = glm::normalize(
            glm::vec3(x, 0.5f - topRadius, z)
        );

        float u = static_cast<float>(i) / segments;

        pushVertex(
            vertices,
            {0.5f * x, 0.f, 0.5f * z},
            normal, u, 0.f
        );

        pushVertex(
            vertices,
            {topRadius * x, 1.f, topRadius * z},
            normal, u, 1.f
        );
    }

    for (int i = 0; i < segments; ++i) {
        unsigned a = 2 * i;
        unsigned b = a + 1;
        unsigned c = a + 2;
        unsigned d = a + 3;

        indices.insert(indices.end(), {
            a, b, c,
            b, d, c
        });
    }

    unsigned center =
        static_cast<unsigned>(vertices.size() / 8);

    pushVertex(
        vertices,
        {0.f, 1.f, 0.f},
        {0.f, 1.f, 0.f},
        0.5f, 0.5f
    );

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.f * PI * i / segments;
        float x = std::cos(angle);
        float z = std::sin(angle);

        pushVertex(
            vertices,
            {topRadius * x, 1.f, topRadius * z},
            {0.f, 1.f, 0.f},
            0.5f + 0.5f * x,
            0.5f + 0.5f * z
        );
    }

    for (int i = 0; i < segments; ++i) {
        unsigned ring = center + 1 + i;

        indices.insert(indices.end(), {
            center, ring + 1, ring
        });
    }

    return Mesh(vertices, indices);
}

Mesh sphere(int stacks, int slices) {
    std::vector<float> vertices;
    std::vector<unsigned> indices;

    for (int i = 0; i <= stacks; ++i) {
        float theta = PI * i / stacks;

        for (int j = 0; j <= slices; ++j) {
            float phi = 2.f * PI * j / slices;

            glm::vec3 normal(
                std::sin(theta) * std::cos(phi),
                std::cos(theta),
                std::sin(theta) * std::sin(phi)
            );

            float u = static_cast<float>(j) / slices;
            float v = 1.f - static_cast<float>(i) / stacks;

            pushVertex(
                vertices,
                normal * 0.5f,
                normal,
                u, v
            );
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            unsigned a = i * (slices + 1) + j;
            unsigned b = a + slices + 1;

            indices.insert(indices.end(), {
                a, a + 1, b,
                a + 1, b + 1, b
            });
        }
    }

    return Mesh(vertices, indices);
}

Mesh torus(int segments, int sides, float tubeRadius) {
    std::vector<float> v;
    std::vector<unsigned> idx;
    for (int i = 0; i <= segments; ++i) {
        float a = 2.f * PI * i / segments;
        for (int j = 0; j <= sides; ++j) {
            float b = 2.f * PI * j / sides;
            float ring = 0.5f + tubeRadius * std::cos(b);
            pushVertex(v, {ring * std::cos(a), tubeRadius * std::sin(b), ring * std::sin(a)},
                       {std::cos(b) * std::cos(a), std::sin(b), std::cos(b) * std::sin(a)});
        }
    }
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < sides; ++j) {
            unsigned a = i * (sides + 1) + j;
            unsigned b = a + sides + 1;
            idx.insert(idx.end(), {a, a + 1, b, a + 1, b + 1, b});
        }
    }
    return Mesh(v, idx);
}

}



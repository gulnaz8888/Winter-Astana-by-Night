#include "Primitives.h"
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

namespace {

constexpr float PI = 3.14159265358979f;

void pushVertex(std::vector<float>& v, const glm::vec3& p, const glm::vec3& n) {
    v.insert(v.end(), {p.x, p.y, p.z, n.x, n.y, n.z});
}

}

namespace Primitives {

Mesh plane() {
    std::vector<float> v;
    const glm::vec3 up(0.f, 1.f, 0.f);
    pushVertex(v, {-0.5f, 0.f, -0.5f}, up);
    pushVertex(v, { 0.5f, 0.f, -0.5f}, up);
    pushVertex(v, { 0.5f, 0.f,  0.5f}, up);
    pushVertex(v, {-0.5f, 0.f,  0.5f}, up);
    std::vector<unsigned> idx = {0, 2, 1, 0, 3, 2};
    return Mesh(v, idx);
}

Mesh cube() {
    struct Face { glm::vec3 n, u, v; };
    const Face faces[6] = {
        {{0.f, 0.f, 1.f},  {1.f, 0.f, 0.f},  {0.f, 1.f, 0.f}},
        {{0.f, 0.f, -1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}},
        {{1.f, 0.f, 0.f},  {0.f, 0.f, -1.f}, {0.f, 1.f, 0.f}},
        {{-1.f, 0.f, 0.f}, {0.f, 0.f, 1.f},  {0.f, 1.f, 0.f}},
        {{0.f, 1.f, 0.f},  {1.f, 0.f, 0.f},  {0.f, 0.f, -1.f}},
        {{0.f, -1.f, 0.f}, {1.f, 0.f, 0.f},  {0.f, 0.f, 1.f}},
    };
    std::vector<float> v;
    std::vector<unsigned> idx;
    for (const Face& f : faces) {
        glm::vec3 c = f.n * 0.5f + glm::vec3(0.f, 0.5f, 0.f);
        glm::vec3 p[4] = {
            c - f.u * 0.5f - f.v * 0.5f,
            c + f.u * 0.5f - f.v * 0.5f,
            c + f.u * 0.5f + f.v * 0.5f,
            c - f.u * 0.5f + f.v * 0.5f};
        unsigned base = (unsigned)(v.size() / 6);
        for (const glm::vec3& q : p) {
            pushVertex(v, q, f.n);
        }
        idx.insert(idx.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
    }
    return Mesh(v, idx);
}

Mesh cylinder(int segments, float topRadius) {
    std::vector<float> v;
    std::vector<unsigned> idx;
    for (int i = 0; i <= segments; ++i) {
        float a = 2.f * PI * i / segments;
        float x = std::cos(a);
        float z = std::sin(a);
        glm::vec3 n = glm::normalize(glm::vec3(x, 0.5f - topRadius, z));
        pushVertex(v, {0.5f * x, 0.f, 0.5f * z}, n);
        pushVertex(v, {topRadius * x, 1.f, topRadius * z}, n);
    }
    for (int i = 0; i < segments; ++i) {
        unsigned a = 2 * i;
        unsigned b = 2 * i + 1;
        unsigned c = 2 * i + 2;
        unsigned d = 2 * i + 3;
        idx.insert(idx.end(), {a, b, c, b, d, c});
    }
    unsigned center = (unsigned)(v.size() / 6);
    pushVertex(v, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f});
    for (int i = 0; i <= segments; ++i) {
        float a = 2.f * PI * i / segments;
        pushVertex(v, {topRadius * std::cos(a), 1.f, topRadius * std::sin(a)}, {0.f, 1.f, 0.f});
    }
    for (int i = 0; i < segments; ++i) {
        unsigned r = center + 1 + i;
        idx.insert(idx.end(), {center, r + 1, r});
    }
    return Mesh(v, idx);
}

Mesh sphere(int stacks, int slices) {
    std::vector<float> v;
    std::vector<unsigned> idx;
    for (int i = 0; i <= stacks; ++i) {
        float theta = PI * i / stacks;
        for (int j = 0; j <= slices; ++j) {
            float phi = 2.f * PI * j / slices;
            glm::vec3 n(std::sin(theta) * std::cos(phi), std::cos(theta), std::sin(theta) * std::sin(phi));
            pushVertex(v, n * 0.5f, n);
        }
    }
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            unsigned a = i * (slices + 1) + j;
            unsigned b = a + slices + 1;
            idx.insert(idx.end(), {a, a + 1, b, a + 1, b + 1, b});
        }
    }
    return Mesh(v, idx);
}

}

#include "Scene.h"
#include <cmath>
#include "Primitives.h"

namespace {

constexpr float PI = 3.14159265358979f;
const glm::vec3 paleBuilding(0.74f, 0.79f, 0.88f);
const glm::vec3 whiteBuilding(0.90f, 0.93f, 0.98f);
const glm::vec3 windowColor(1.f, 0.80f, 0.45f);
const glm::vec3 lampColor(1.f, 0.88f, 0.62f);

struct Tower {
    float x, z, w, d, h;
};

}

Scene::Scene()
    : plane(Primitives::plane()),
      cube(Primitives::cube()),
      cylinder(Primitives::cylinder()),
      taper(Primitives::cylinder(32, 0.25f)),
      cone(Primitives::cylinder(32, 0.f)),
      sphere(Primitives::sphere()) {
    buildGround();
    buildAvenue();
    buildLamps();
    buildSkyline();
    buildMosque();
    buildKhanShatyr();
    buildBaiterek();
}

Node* Scene::add(Node& parent, const Mesh& mesh, glm::vec3 position, glm::vec3 scale,
                 glm::vec3 color, glm::vec3 emissive) {
    auto node = std::make_unique<Node>();
    node->mesh = &mesh;
    node->position = position;
    node->scale = scale;
    node->color = color;
    node->emissive = emissive;
    return parent.addChild(std::move(node));
}

void Scene::buildGround() {
    add(root, plane, {0.f, -0.05f, 0.f}, {600.f, 1.f, 600.f}, {0.55f, 0.63f, 0.80f});
    add(root, plane, {0.f, 0.f, 10.f}, {14.f, 1.f, 100.f}, {0.16f, 0.19f, 0.28f});
    for (int i = 0; i < 10; ++i) {
        add(root, plane, {0.f, 0.05f, 52.f - i * 10.f}, {0.35f, 1.f, 4.5f}, {0.85f, 0.88f, 0.95f});
    }
    add(root, cylinder, {0.f, 0.f, -55.f}, {30.f, 0.2f, 30.f}, {0.70f, 0.76f, 0.90f});
}

void Scene::buildAvenue() {
    const float heights[8] = {9.f, 13.f, 8.f, 15.f, 10.f, 12.f, 9.f, 14.f};
    for (int i = 0; i < 8; ++i) {
        float side = (i % 2 == 0) ? -1.f : 1.f;
        float h = heights[i];
        float z = 34.f - (i / 2) * 18.f;
        Node* building = add(root, cube, {side * 17.f, 0.f, z}, {8.f, h, 12.f}, paleBuilding);
        for (float y = 2.5f; y < h - 2.f; y += 3.5f) {
            for (int c = 0; c < 3; ++c) {
                if ((i * 7 + (int)y * 3 + c * 5) % 4 == 0) {
                    continue;
                }
                add(*building, cube, {-side * 4.05f, y, (c - 1) * 3.5f}, {0.1f, 1.3f, 1.8f},
                    windowColor, windowColor * 0.9f);
            }
        }
    }
}

void Scene::buildLamps() {
    for (int i = 0; i < 7; ++i) {
        float z = 50.f - i * 13.f;
        for (float side : {-1.f, 1.f}) {
            Node* pole = add(root, cylinder, {side * 8.f, 0.f, z}, {0.35f, 7.5f, 0.35f}, {0.20f, 0.22f, 0.30f});
            add(*pole, sphere, {0.f, 7.7f, 0.f}, {1.4f, 1.4f, 1.4f}, lampColor, lampColor);
        }
    }
}

void Scene::buildSkyline() {
    const Tower back[] = {
        {-95.f, -130.f, 14.f, 14.f, 42.f}, {-78.f, -125.f, 12.f, 12.f, 58.f},
        {-60.f, -135.f, 16.f, 14.f, 36.f}, {-42.f, -120.f, 12.f, 12.f, 50.f},
        {-30.f, -128.f, 10.f, 10.f, 30.f}, {30.f, -125.f, 10.f, 10.f, 34.f},
        {44.f, -132.f, 14.f, 12.f, 52.f},  {62.f, -122.f, 12.f, 12.f, 40.f},
        {80.f, -130.f, 16.f, 14.f, 62.f},  {98.f, -125.f, 12.f, 12.f, 38.f}};
    for (const Tower& t : back) {
        Node* tower = add(root, cube, {t.x, 0.f, t.z}, {t.w, t.h, t.d}, paleBuilding);
        if (t.h >= 50.f) {
            add(*tower, taper, {0.f, t.h, 0.f}, {1.2f, 10.f, 1.2f}, paleBuilding);
        }
    }
    const Tower mid[] = {
        {-85.f, -90.f, 12.f, 14.f, 26.f}, {-68.f, -98.f, 10.f, 12.f, 34.f},
        {-50.f, -88.f, 10.f, 12.f, 22.f}, {52.f, -92.f, 12.f, 12.f, 30.f},
        {70.f, -86.f, 10.f, 12.f, 24.f},  {88.f, -96.f, 14.f, 14.f, 36.f}};
    for (const Tower& t : mid) {
        add(root, cube, {t.x, 0.f, t.z}, {t.w, t.h, t.d}, whiteBuilding);
    }
}

void Scene::buildMosque() {
    Node* base = add(root, cube, {45.f, 0.f, -60.f}, {26.f, 7.f, 22.f}, whiteBuilding);
    add(*base, cylinder, {0.f, 7.f, 0.f}, {10.f, 4.f, 10.f}, whiteBuilding);
    add(*base, sphere, {0.f, 11.f, 0.f}, {11.f, 11.f, 11.f}, whiteBuilding);
    add(*base, taper, {0.f, 16.f, 0.f}, {0.6f, 5.f, 0.6f}, whiteBuilding);
    for (float sx : {-1.f, 1.f}) {
        for (float sz : {-1.f, 1.f}) {
            Node* minaret = add(*base, taper, {sx * 14.f, 0.f, sz * 10.f}, {2.f, 26.f, 2.f}, whiteBuilding);
            add(*minaret, cylinder, {0.f, 20.f, 0.f}, {2.6f, 0.4f, 2.6f}, whiteBuilding);
            add(*minaret, cone, {0.f, 26.f, 0.f}, {1.6f, 4.f, 1.6f}, whiteBuilding);
        }
    }
}

void Scene::buildKhanShatyr() {
    Node* tent = add(root, cone, {-62.f, 0.f, -62.f}, {44.f, 34.f, 44.f}, whiteBuilding);
    add(*tent, taper, {0.f, 34.f, 0.f}, {0.8f, 10.f, 0.8f}, whiteBuilding);
}

void Scene::buildBaiterek() {
    Node* tower = add(root, taper, {0.f, 0.2f, -55.f}, {8.f, 30.f, 8.f}, whiteBuilding);
    for (int i = 0; i < 8; ++i) {
        float a = 2.f * PI * i / 8.f;
        Node* branch = add(*tower, taper, {2.5f * std::sin(a), 6.f, 2.5f * std::cos(a)},
                           {1.6f, 28.f, 1.6f}, whiteBuilding);
        branch->rotation = {0.3f, a, 0.f};
    }
    add(*tower, taper, {0.f, 43.5f, 0.f}, {0.8f, 10.f, 0.8f}, whiteBuilding);

    auto pivot = std::make_unique<Node>();
    pivot->position = {0.f, 36.f, 0.f};
    ballPivot = tower->addChild(std::move(pivot));
    add(*ballPivot, sphere, {0.f, 0.f, 0.f}, {16.f, 16.f, 16.f}, {0.93f, 0.95f, 1.f}, {0.25f, 0.30f, 0.40f});
    for (int i = 0; i < 12; ++i) {
        float a = 2.f * PI * i / 12.f;
        float y = ((i % 3) - 1) * 3.5f;
        float r = std::sqrt(64.f - y * y) + 0.2f;
        add(*ballPivot, sphere, {r * std::sin(a), y, r * std::cos(a)}, {1.1f, 1.1f, 1.1f},
            lampColor, {1.f, 0.80f, 0.40f});
    }
}

void Scene::update(float time) {
    ballPivot->rotation.y = time * 0.8f;
}

void Scene::draw(const Shader& shader) const {
    root.draw(shader, glm::mat4(1.f));
}

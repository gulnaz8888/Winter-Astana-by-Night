#include "Scene.h"
#include "Primitives.h"

Scene::Scene()
    : plane(Primitives::plane()),
      cube(Primitives::cube()),
      cylinder(Primitives::cylinder()),
      taper(Primitives::cylinder(32, 0.25f)),
      sphere(Primitives::sphere()) {
    buildGround();
    buildBuildings();
    buildLamps();
    buildBaiterek();
    buildMoon();
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
    add(root, plane, {0.f, -0.05f, 0.f}, {400.f, 1.f, 400.f}, {0.80f, 0.85f, 0.95f});
    add(root, plane, {0.f, 0.f, -20.f}, {14.f, 1.f, 160.f}, {0.07f, 0.07f, 0.09f});
    for (int i = 0; i < 12; ++i) {
        add(root, plane, {0.f, 0.05f, 55.f - i * 12.f}, {0.4f, 1.f, 5.f}, {0.9f, 0.9f, 0.85f});
    }
}

void Scene::buildBuildings() {
    const float heights[10] = {12.f, 16.f, 10.f, 20.f, 14.f, 18.f, 11.f, 22.f, 13.f, 17.f};
    for (int i = 0; i < 10; ++i) {
        float side = (i % 2 == 0) ? -1.f : 1.f;
        float h = heights[i];
        float z = 20.f - (i / 2) * 16.f;
        Node* building = add(root, cube, {side * 15.f, 0.f, z}, {8.f, h, 12.f}, {0.22f, 0.25f, 0.36f});
        for (float y = 3.f; y < h - 2.f; y += 4.f) {
            for (int c = 0; c < 3; ++c) {
                if ((i * 7 + (int)y * 3 + c * 5) % 4 == 0) {
                    continue;
                }
                add(*building, cube, {-side * 4.05f, y, (c - 1) * 3.5f}, {0.1f, 1.4f, 1.8f},
                    {1.f, 0.78f, 0.4f}, {1.f, 0.78f, 0.4f});
            }
        }
    }
}

void Scene::buildLamps() {
    for (int i = 0; i < 8; ++i) {
        float z = 45.f - i * 18.f;
        for (float side : {-1.f, 1.f}) {
            Node* pole = add(root, cylinder, {side * 8.f, 0.f, z}, {0.3f, 7.f, 0.3f}, {0.15f, 0.15f, 0.18f});
            add(*pole, sphere, {0.f, 7.3f, 0.f}, {1.2f, 1.2f, 1.2f}, {1.f, 0.95f, 0.8f}, {1.f, 0.9f, 0.7f});
        }
    }
}

void Scene::buildBaiterek() {
    Node* tower = add(root, taper, {0.f, 0.f, -70.f}, {7.f, 24.f, 7.f}, {0.62f, 0.68f, 0.82f});
    add(*tower, sphere, {0.f, 28.f, 0.f}, {11.f, 11.f, 11.f}, {1.f, 0.83f, 0.42f}, {1.f, 0.75f, 0.3f});

    auto pivot = std::make_unique<Node>();
    pivot->position = {0.f, 28.f, 0.f};
    beaconPivot = tower->addChild(std::move(pivot));
    add(*beaconPivot, cube, {8.f, 0.f, 0.f}, {4.f, 0.8f, 0.8f}, {1.f, 0.9f, 0.5f}, {1.f, 0.8f, 0.3f});
    add(*beaconPivot, cube, {-8.f, 0.f, 0.f}, {4.f, 0.8f, 0.8f}, {1.f, 0.9f, 0.5f}, {1.f, 0.8f, 0.3f});
}

void Scene::buildMoon() {
    add(root, sphere, {-70.f, 80.f, -180.f}, {16.f, 16.f, 16.f}, {0.9f, 0.93f, 1.f}, {0.85f, 0.9f, 1.f});
}

void Scene::update(float time) {
    beaconPivot->rotation.y = time * 1.2f;
}

void Scene::draw(const Shader& shader) const {
    root.draw(shader, glm::mat4(1.f));
}

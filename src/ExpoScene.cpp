#include "ExpoScene.h"
#include <algorithm>
#include <cmath>
#include "Primitives.h"

namespace {

constexpr float PI = 3.14159265358979f;
const glm::vec3 sphereCenter(6.f, 38.f, -42.f);
constexpr float sphereRadius = 30.f;
const glm::vec3 iceWhite(0.80f, 0.88f, 1.f);
const glm::vec3 warmLight(1.f, 0.78f, 0.35f);
const glm::vec3 treeLights[5] = {
    {1.f, 0.85f, 0.2f}, {1.f, 1.f, 1.f}, {0.3f, 1.f, 0.35f}, {1.f, 0.25f, 0.7f}, {1.f, 0.3f, 0.2f}};

struct Tier {
    float base, height, diameter;
};

const Tier tiers[5] = {
    {1.5f, 10.f, 14.f}, {7.f, 9.f, 11.5f}, {12.5f, 8.f, 9.f}, {17.5f, 7.f, 6.5f}, {22.f, 6.f, 4.f}};

float hash(float x) {
    float s = std::sin(x * 12.9898f) * 43758.5453f;
    return s - std::floor(s);
}

}

ExpoScene::ExpoScene()
    : plane(Primitives::plane()),
      cube(Primitives::cube()),
      cylinder(Primitives::cylinder()),
      taper(Primitives::cylinder(32, 0.25f)),
      cone(Primitives::cylinder(32, 0.f)),
      sphere(Primitives::sphere()),
      bigSphere(Primitives::sphere(48, 64)),
      torus(Primitives::torus()) {
    buildGround();
    buildPodium();
    buildNurAlem();
    buildTree();
    buildOrnament({-2.f, 0.f, 14.f}, 5.5f, iceWhite, {1.f, 0.15f, 0.12f});
    buildOrnament({18.f, 0.f, 4.f}, 3.5f, {0.75f, 0.85f, 1.f}, {0.55f, 0.35f, 1.f});
    buildOrnament({31.f, 0.f, 4.f}, 3.5f, {0.75f, 0.85f, 1.f}, {0.55f, 0.35f, 1.f});
    buildArches(-46.f, -26.f, -8.f);
    buildArches(6.f, 38.f, -8.f);
    buildBench({23.f, 0.f, 0.f});
    buildCrowd();
}

Node* ExpoScene::add(Node& parent, const Mesh& mesh, glm::vec3 position, glm::vec3 scale,
                 glm::vec3 color, glm::vec3 emissive) {
    auto node = std::make_unique<Node>();
    node->mesh = &mesh;
    node->position = position;
    node->scale = scale;
    node->color = color;
    node->emissive = emissive;
    return parent.addChild(std::move(node));
}

Node* ExpoScene::addGroup(Node& parent, glm::vec3 position) {
    auto node = std::make_unique<Node>();
    node->position = position;
    return parent.addChild(std::move(node));
}

Node* ExpoScene::addRod(Node& parent, glm::vec3 from, glm::vec3 to, float thickness, glm::vec3 color) {
    glm::vec3 d = to - from;
    Node* rod = add(parent, cylinder, from, {thickness, glm::length(d), thickness}, color, color);
    rod->rotation.z = -std::atan2(d.x, d.y);
    return rod;
}

void ExpoScene::buildGround() {
    add(root, plane, {0.f, -0.05f, 0.f}, {700.f, 1.f, 700.f}, {0.80f, 0.83f, 0.92f}, {0.20f, 0.21f, 0.26f});
    add(root, plane, {6.f, 0.02f, -10.f}, {130.f, 1.f, 70.f}, {0.88f, 0.90f, 0.96f}, {0.30f, 0.31f, 0.36f});
    for (int i = 0; i < 14; ++i) {
        float x = -60.f + hash(i * 3.1f) * 140.f;
        float z = -20.f + hash(i * 5.7f) * 45.f;
        add(root, plane, {x, 0.04f, z}, {6.f + hash(i * 7.3f) * 10.f, 1.f, 3.f + hash(i * 2.9f) * 5.f},
            {0.72f, 0.75f, 0.86f}, {0.14f, 0.15f, 0.19f});
    }
}

void ExpoScene::buildPodium() {
    const glm::vec3 wall(0.30f, 0.31f, 0.36f);
    const glm::vec3 dark(0.05f, 0.05f, 0.07f);
    add(root, cube, {6.f, 0.f, -42.f}, {120.f, 10.f, 30.f}, wall);
    add(root, cube, {6.f, 10.f, -41.f}, {126.f, 0.8f, 34.f}, {0.26f, 0.27f, 0.32f});
    add(root, cube, {6.f, 1.5f, -26.9f}, {100.f, 5.5f, 0.3f}, {0.10f, 0.08f, 0.06f}, {0.75f, 0.45f, 0.12f});
    for (float x = -42.f; x <= 54.f; x += 6.f) {
        add(root, cube, {x, 1.5f, -26.7f}, {0.35f, 5.5f, 0.3f}, dark);
    }
    add(root, cube, {-51.f, 1.5f, -26.8f}, {14.f, 5.5f, 0.3f}, {0.2f, 0.25f, 0.05f}, {0.4f, 0.55f, 0.12f});
    for (float x = 40.f; x <= 66.f; x += 1.6f) {
        add(root, sphere, {x, 10.9f, -24.5f}, glm::vec3(0.5f), warmLight, warmLight);
    }
}

void ExpoScene::buildNurAlem() {
    Node* globe = add(root, bigSphere, sphereCenter, glm::vec3(sphereRadius * 2.f),
                      {0.04f, 0.055f, 0.085f}, {0.03f, 0.04f, 0.06f});
    int row = 0;
    for (float elev = -70.f; elev <= 77.f; elev += 7.f, ++row) {
        float e = glm::radians(elev);
        int count = std::max(8, (int)(52.f * std::cos(e)));
        for (int k = 0; k < count; ++k) {
            float psi = 2.f * PI * (k + 0.5f * (row % 2)) / count;
            glm::vec3 n(std::cos(e) * std::sin(psi), std::sin(e), std::cos(e) * std::cos(psi));
            glm::vec3 color = iceWhite;
            if (elev < -12.f) {
                color = ((row + k) % 3 == 0) ? glm::vec3(0.55f, 0.9f, 0.25f) : glm::vec3(1.f, 0.75f, 0.3f);
            }
            Node* facet = add(*globe, cube, n * (sphereRadius + 0.1f),
                              {0.7f, 1.2f + 2.0f * std::cos(e), 0.25f}, color, color * 0.95f);
            facet->rotation = {-e, psi, (k % 2 == 0) ? 0.65f : -0.65f};
        }
    }
    add(root, torus, sphereCenter + glm::vec3(0.f, -6.f, 0.f), glm::vec3(58.8f), {0.25f, 0.27f, 0.30f}, {0.10f, 0.10f, 0.12f});
    add(root, taper, sphereCenter + glm::vec3(0.f, sphereRadius - 1.f, 0.f), {1.4f, 20.f, 1.4f},
        {0.6f, 0.6f, 0.55f}, {0.35f, 0.34f, 0.30f});
}

void ExpoScene::buildTree() {
    Node* tree = add(root, cylinder, {-15.f, 0.f, -2.f}, {1.8f, 3.f, 1.8f}, {0.25f, 0.14f, 0.08f});
    for (const Tier& t : tiers) {
        add(*tree, cone, {0.f, t.base, 0.f}, {t.diameter, t.height, t.diameter},
            {0.04f, 0.22f, 0.10f}, {0.02f, 0.10f, 0.05f});
    }
    int idx = 0;
    for (int i = 0; i < 5; ++i) {
        const Tier& t = tiers[i];
        for (int k = 0; k < 34; ++k) {
            float f = 0.06f + 0.9f * k / 33.f;
            float a = f * PI * 9.f + i;
            float r = t.diameter * 0.5f * (1.f - f) + 0.3f;
            glm::vec3 c = treeLights[idx % 5];
            Node* bulb = add(*tree, sphere, {r * std::cos(a), t.base + f * t.height, r * std::sin(a)},
                             glm::vec3(0.55f), c, c * 1.1f);
            twinkles.push_back({bulb, bulb->emissive, hash(idx * 1.37f) * 2.f * PI});
            ++idx;
        }
        for (int k = 0; k < 4; ++k) {
            float f = 0.3f + 0.1f * (k % 2);
            float a = k * PI / 2.f + i;
            float r = t.diameter * 0.5f * (1.f - f) + 0.4f;
            add(*tree, sphere, {r * std::cos(a), t.base + f * t.height, r * std::sin(a)}, glm::vec3(0.85f),
                {1.f, 0.8f, 0.1f}, {1.f, 0.75f, 0.1f});
        }
    }
    starPivot = addGroup(*tree, {0.f, 28.4f, 0.f});
    add(*starPivot, sphere, {0.f, 0.f, 0.f}, glm::vec3(1.8f), {1.f, 0.95f, 0.7f}, {1.f, 0.95f, 0.7f});
    for (int i = 0; i < 8; ++i) {
        Node* spike = add(*starPivot, cube, {0.f, 0.f, 0.f}, {0.35f, (i % 2 == 0) ? 2.4f : 1.5f, 0.35f},
                          {1.f, 0.95f, 0.6f}, {1.f, 0.95f, 0.6f});
        spike->rotation.z = i * PI / 4.f;
    }
}

void ExpoScene::buildOrnament(glm::vec3 position, float diameter, glm::vec3 ring, glm::vec3 core) {
    Node* pivot = addGroup(root, position + glm::vec3(0.f, diameter * 0.5f, 0.f));
    spinners.push_back(pivot);
    add(*pivot, sphere, {0.f, 0.f, 0.f}, glm::vec3(diameter * 0.3f), core, core);
    for (int k = 0; k < 6; ++k) {
        Node* meridian = add(*pivot, torus, {0.f, 0.f, 0.f}, glm::vec3(diameter), ring, ring);
        meridian->rotation = {PI * 0.5f, k * PI / 6.f, 0.f};
    }
    for (float h : {-0.36f, -0.2f, 0.f, 0.2f, 0.36f}) {
        float d = diameter * std::sqrt(1.f - 4.f * h * h);
        add(*pivot, torus, {0.f, h * diameter, 0.f}, glm::vec3(d), ring, ring);
    }
    const glm::vec3 gold(0.7f, 0.55f, 0.2f);
    add(*pivot, cylinder, {0.f, diameter * 0.5f - 0.05f, 0.f},
        {diameter * 0.16f, diameter * 0.1f, diameter * 0.16f}, gold, gold * 0.3f);
}

void ExpoScene::buildArches(float x0, float x1, float z) {
    const float w = 4.5f;
    int count = (int)((x1 - x0) / w);
    Node* row = addGroup(root, {0.f, 0.f, 0.f});
    for (int i = 0; i < count; ++i) {
        float x = x0 + i * w;
        float h = 3.2f + 1.8f * std::fabs(std::sin(i * 1.7f + x0));
        glm::vec3 a(x, 0.1f, z);
        glm::vec3 peak(x + w * 0.5f, h, z);
        glm::vec3 b(x + w, 0.1f, z);
        addRod(*row, a, peak, 0.14f, iceWhite);
        addRod(*row, peak, b, 0.14f, iceWhite);
        addRod(*row, {x + w * 0.25f, h * 0.5f, z}, {x + w * 0.75f, h * 0.5f, z}, 0.1f, iceWhite);
    }
    addRod(*row, {x0, 0.1f, z}, {x0 + count * w, 0.1f, z}, 0.14f, iceWhite);
}

void ExpoScene::buildBench(glm::vec3 position) {
    const glm::vec3 wood(0.35f, 0.20f, 0.12f);
    Node* bench = addGroup(root, position);
    add(*bench, cube, {0.f, 1.1f, 0.f}, {4.2f, 0.25f, 1.3f}, wood);
    add(*bench, cube, {0.f, 1.6f, -0.65f}, {4.2f, 1.2f, 0.2f}, wood);
    add(*bench, cube, {-1.8f, 0.f, 0.f}, {0.25f, 1.1f, 1.1f}, wood * 0.7f);
    add(*bench, cube, {1.8f, 0.f, 0.f}, {0.25f, 1.1f, 1.1f}, wood * 0.7f);
}

void ExpoScene::buildPerson(glm::vec3 position, float height, glm::vec3 coat, glm::vec3 hat, float facing) {
    const float h = height;
    const glm::vec3 pants(0.10f, 0.10f, 0.16f);
    const glm::vec3 skin(0.55f, 0.40f, 0.34f);
    Node* g = addGroup(root, position);
    g->rotation.y = facing;
    for (float s : {-1.f, 1.f}) {
        add(*g, cylinder, {s * 0.07f * h, 0.f, 0.f}, {0.1f * h, 0.42f * h, 0.1f * h}, pants);
        add(*g, cylinder, {s * 0.17f * h, 0.5f * h, 0.f}, {0.075f * h, 0.3f * h, 0.075f * h}, coat, coat * 0.22f);
    }
    add(*g, cylinder, {0.f, 0.36f * h, 0.f}, {0.3f * h, 0.44f * h, 0.22f * h}, coat, coat * 0.22f);
    add(*g, sphere, {0.f, 0.88f * h, 0.f}, glm::vec3(0.19f * h), skin);
    add(*g, sphere, {0.f, 0.93f * h, 0.f}, {0.23f * h, 0.15f * h, 0.23f * h}, hat, hat * 0.22f);
}

void ExpoScene::buildCrowd() {
    buildPerson({-7.f, 0.f, 10.f}, 4.2f, {0.40f, 0.34f, 0.55f}, {0.30f, 0.25f, 0.40f}, 0.4f);
    buildPerson({-12.f, 0.f, 13.f}, 2.6f, {0.85f, 0.60f, 0.15f}, {0.30f, 0.20f, 0.10f}, -0.3f);
    buildPerson({-10.f, 0.f, 14.f}, 2.7f, {0.70f, 0.20f, 0.55f}, {0.45f, 0.10f, 0.35f}, 0.2f);
    buildPerson({5.f, 0.f, 13.f}, 4.4f, {0.30f, 0.25f, 0.80f}, {0.25f, 0.20f, 0.65f}, -0.5f);
    buildPerson({9.f, 0.f, 12.f}, 4.0f, {0.90f, 0.15f, 0.10f}, {0.80f, 0.10f, 0.08f}, 0.6f);
    buildPerson({11.5f, 0.f, 20.f}, 2.8f, {0.80f, 0.10f, 0.25f}, {0.70f, 0.08f, 0.20f}, 3.0f);
    add(root, cylinder, {3.2f, 0.f, 16.f}, {1.8f, 0.3f, 1.8f}, {0.8f, 0.1f, 0.08f}, {0.2f, 0.02f, 0.02f});
}

void ExpoScene::update(float time) {
    starPivot->rotation.y = time * 1.2f;
    for (size_t i = 0; i < spinners.size(); ++i) {
        spinners[i]->rotation.y = time * (0.4f + 0.1f * i);
    }
    for (Twinkle& t : twinkles) {
        t.node->emissive = t.base * (0.55f + 0.45f * std::sin(time * 3.f + t.phase));
    }
}

void ExpoScene::draw(const Shader& shader) const {
    root.draw(shader, glm::mat4(1.f));
}

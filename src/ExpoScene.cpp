
#include "ExpoScene.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "Primitives.h"

namespace {

constexpr float PI = 3.14159265358979323846f;

const glm::vec3 sphereCenter(6.f, 38.f, -42.f);
constexpr float sphereRadius = 30.f;

const glm::vec3 snowWhite(0.86f, 0.92f, 1.f);
const glm::vec3 warmLight(1.f, 0.76f, 0.38f);
const glm::vec3 blueLight(0.32f, 0.72f, 1.f);

float hash(float x) {
    float s = std::sin(x * 12.9898f) * 43758.5453f;
    return s - std::floor(s);
}

}

ExpoScene::ExpoScene()
    : plane(Primitives::plane()),
      cube(Primitives::cube()),
      cylinder(Primitives::cylinder(12)),
      taper(Primitives::cylinder(12, 0.25f)),
      cone(Primitives::cylinder(12, 0.f)),
      sphere(Primitives::sphere(12, 16)),
      bigSphere(Primitives::sphere(48, 64)),
      torus(Primitives::torus()) {

    podiumGlass = std::make_unique<Texture>(
        "assets/textures/blue_glass.png"
    );

    expoGlass = std::make_unique<Texture>(
        "assets/textures/blue_glass.png"
    );

    pineTexture = std::make_unique<Texture>(
        "assets/textures/pine_needles.png"
    );

    buildingGlass = std::make_unique<Texture>(
        "assets/textures/blue_glass.png"
    );

    buildingBrick = std::make_unique<Texture>(
        "assets/textures/brick.png"
    );

    buildGround();
    buildPodium();
    buildNurAlem();
    buildTree();

    buildOrnament(
        {-2.f, 0.f, 14.f},
        5.5f,
        snowWhite,
        {1.f, 0.15f, 0.12f}
    );

    buildOrnament(
        {18.f, 0.f, 4.f},
        3.5f,
        snowWhite,
        {0.55f, 0.35f, 1.f}
    );

    buildOrnament(
        {31.f, 0.f, 4.f},
        3.5f,
        snowWhite,
        {0.55f, 0.35f, 1.f}
    );

    buildArches(-46.f, -26.f, -8.f);
    buildArches(6.f, 38.f, -8.f);

    buildBench({23.f, 0.f, 0.f});
    buildCrowd();

    buildExpoLayout();
    buildDistrict();
    buildWinterPark();
}

Node* ExpoScene::add(
    Node& parent,
    const Mesh& mesh,
    glm::vec3 position,
    glm::vec3 scale,
    glm::vec3 color,
    glm::vec3 emissive
) {
    auto node = std::make_unique<Node>();

    node->mesh = &mesh;
    node->position = position;
    node->scale = scale;
    node->color = color;
    node->emissive = emissive;

    return parent.addChild(std::move(node));
}

Node* ExpoScene::addGroup(
    Node& parent,
    glm::vec3 position
) {
    auto node = std::make_unique<Node>();
    node->position = position;

    return parent.addChild(std::move(node));
}

Node* ExpoScene::addRod(
    Node& parent,
    glm::vec3 from,
    glm::vec3 to,
    float thickness,
    glm::vec3 color
) {
    glm::vec3 delta = to - from;

    Node* rod = add(
        parent,
        cylinder,
        from,
        {
            thickness,
            glm::length(delta),
            thickness
        },
        color,
        color * 0.25f
    );

    rod->rotation.z = -std::atan2(
        delta.x,
        delta.y
    );

    return rod;
}

// =====================================================
// GROUND
// =====================================================

void ExpoScene::buildGround() {
    add(
        root,
        plane,
        {0.f, -0.05f, 0.f},
        {700.f, 1.f, 700.f},
        {0.80f, 0.84f, 0.92f},
        {0.18f, 0.19f, 0.24f}
    );

    add(
        root,
        plane,
        {6.f, 0.02f, -10.f},
        {130.f, 1.f, 70.f},
        {0.88f, 0.91f, 0.97f},
        {0.25f, 0.27f, 0.32f}
    );
}

// =====================================================
// CURVED EXPO PAVILION
// =====================================================

void ExpoScene::buildPodium() {
    const glm::vec3 darkGlass(
        0.08f, 0.18f, 0.27f
    );

    const glm::vec3 glassFrame(
        0.18f, 0.27f, 0.34f
    );

    const glm::vec3 roofColor(
        0.52f, 0.59f, 0.66f
    );

    add(
        root,
        cube,
        {6.f, 0.f, -42.f},
        {120.f, 9.5f, 28.f},
        {0.055f, 0.09f, 0.15f}
    );

    add(
        root,
        cube,
        {6.f, 9.7f, -42.f},
        {126.f, 0.65f, 34.f},
        roofColor,
        {0.025f, 0.04f, 0.055f}
    );

    constexpr int segments = 32;

    const float xStart = -54.f;
    const float xEnd = 66.f;
    const float segmentWidth =
        (xEnd - xStart) / segments;

    for (int i = 0; i < segments; ++i) {
        float x0 = xStart + i * segmentWidth;
        float x1 = x0 + segmentWidth;
        float xMid = (x0 + x1) * 0.5f;

        float normalized =
            (xMid - 6.f) / 60.f;

        float curve =
            3.0f * (1.f - normalized * normalized);

        float z = -26.7f + curve;

        Node* glass = add(
            root,
            cube,
            {xMid, 4.6f, z},
            {
                segmentWidth + 0.06f,
                8.6f,
                0.12f
            },
            darkGlass,
            {0.008f, 0.025f, 0.035f}
        );

        glass->texture = podiumGlass.get();
        glass->uvScale = {1.f, 1.f, 1.f};

        float derivative =
            -6.f * normalized / 60.f;

        glass->rotation.y =
            -std::atan(derivative);

        if (i % 2 == 0) {
            add(
                root,
                cube,
                {xMid, 4.6f, z + 0.12f},
                {0.12f, 8.8f, 0.18f},
                glassFrame
            );
        }

        if (i % 2 == 0) {
            add(
                root,
                cube,
                {xMid, 6.8f, z + 0.14f},
                {
                    segmentWidth * 2.f,
                    0.10f,
                    0.14f
                },
                glassFrame
            );
        }
    }

    for (int i = 0; i < segments; ++i) {
        float xMid =
            xStart + (i + 0.5f) * segmentWidth;

        float normalized =
            (xMid - 6.f) / 60.f;

        float curve =
            3.f * (1.f - normalized * normalized);

        add(
            root,
            cube,
            {xMid, 9.5f, -26.7f + curve},
            {segmentWidth + 0.15f, 0.35f, 0.7f},
            roofColor
        );
    }

    add(
        root,
        cube,
        {6.f, 0.15f, -22.5f},
        {15.f, 0.30f, 5.f},
        {0.30f, 0.37f, 0.44f}
    );

    add(
        root,
        cube,
        {6.f, 4.7f, -22.4f},
        {16.f, 0.45f, 5.5f},
        roofColor
    );

    for (float x : {-1.f, 13.f}) {
        add(
            root,
            cube,
            {x, 2.4f, -20.0f},
            {0.40f, 4.8f, 0.45f},
            glassFrame
        );
    }

    for (float x : {2.5f, 6.f, 9.5f}) {
        add(
            root,
            cube,
            {x, 2.1f, -19.85f},
            {3.1f, 4.2f, 0.12f},
            {0.05f, 0.13f, 0.20f},
            {0.008f, 0.02f, 0.03f}
        );
    }

    add(
        root,
        cube,
        {6.f, 4.35f, -19.7f},
        {13.f, 0.12f, 0.12f},
        {0.92f, 0.72f, 0.43f},
        {0.22f, 0.14f, 0.06f}
    );
}

// =====================================================
// SMOOTH NUR ALEM SPHERE
// =====================================================

void ExpoScene::buildNurAlem() {
    Node* globe = add(
        root,
        bigSphere,
        sphereCenter,
        glm::vec3(sphereRadius * 2.f),
        {0.09f, 0.24f, 0.34f},
        {0.012f, 0.045f, 0.065f}
    );

    globe->texture = expoGlass.get();
    globe->uvScale = {3.f, 2.f, 1.f};

    for (int i = 0; i < 9; ++i) {
        float relativeY = -24.f + i * 6.f;

        float ringRadius = std::sqrt(
            std::max(
                0.f,
                sphereRadius * sphereRadius -
                relativeY * relativeY
            )
        );

        add(
            root,
            torus,
            sphereCenter +
                glm::vec3(0.f, relativeY, 0.f),
            glm::vec3(ringRadius * 2.002f),
            {0.12f, 0.31f, 0.39f},
            {0.012f, 0.055f, 0.075f}
        );
    }

    add(
        root,
        cylinder,
        sphereCenter + glm::vec3(0.f, 29.2f, 0.f),
        {3.2f, 1.f, 3.2f},
        {0.30f, 0.40f, 0.49f}
    );

    add(
        root,
        taper,
        sphereCenter + glm::vec3(0.f, 30.f, 0.f),
        {1.2f, 12.f, 1.2f},
        {0.58f, 0.68f, 0.77f}
    );
}

// NEXT: buildTree()

 // =====================================================
 // MAIN CHRISTMAS TREE
 // =====================================================

void ExpoScene::buildTree() {
    Node* tree = addGroup(
        root,
        {-15.f, 0.f, -2.f}
    );

    add(
        *tree,
        cylinder,
        {0.f, 0.f, 0.f},
        {1.5f, 3.2f, 1.5f},
        {0.25f, 0.16f, 0.10f}
    );

    for (int i = 0; i < 8; ++i) {
        float y = 0.8f + i * 3.1f;
        float radius = 8.5f * (1.f - i / 9.f);

        Node* crown = add(
            *tree,
            cone,
            {0.f, y, 0.f},
            {
                radius * 2.f,
                7.f,
                radius * 2.f
            },
            {0.52f, 0.74f, 0.52f},
            {0.015f, 0.04f, 0.02f}
        );

        crown->texture = pineTexture.get();
        crown->uvScale = {1.4f, 1.f, 1.f};
    }

    const glm::vec3 colors[3] = {
        {1.f, 0.76f, 0.32f},
        {0.35f, 0.72f, 1.f},
        {1.f, 0.35f, 0.62f}
    };

    constexpr int bulbs = 110;

    for (int i = 0; i < bulbs; ++i) {
        float t = i / float(bulbs - 1);
        float y = 1.9f + t * 25.4f;

        float radius =
            8.35f * (1.f - t * 0.90f);

        float angle =
            2.f * PI * (t * 8.f + 0.15f);

        glm::vec3 position(
            radius * std::cos(angle),
            y,
            radius * std::sin(angle)
        );

        glm::vec3 color = colors[i % 3];

        Node* bulb = add(
            *tree,
            sphere,
            position,
            glm::vec3(0.38f),
            color,
            color * 0.85f
        );

        twinkles.push_back({
            bulb,
            bulb->emissive,
            hash(i * 1.37f) * 2.f * PI
        });
    }

    starPivot = addGroup(
        *tree,
        {0.f, 29.f, 0.f}
    );

    add(
        *starPivot,
        sphere,
        {0.f, 0.f, 0.f},
        glm::vec3(1.15f),
        {1.f, 0.85f, 0.40f},
        {0.55f, 0.38f, 0.12f}
    );

    for (int i = 0; i < 8; ++i) {
        Node* ray = add(
            *starPivot,
            cube,
            {0.f, 0.f, 0.f},
            {
                0.20f,
                i % 2 == 0 ? 2.f : 1.35f,
                0.20f
            },
            {1.f, 0.86f, 0.42f},
            {0.48f, 0.32f, 0.10f}
        );

        ray->rotation.z = i * PI / 4.f;
    }
}

// =====================================================
// DECORATIVE ORNAMENTS
// =====================================================

void ExpoScene::buildOrnament(
    glm::vec3 position,
    float diameter,
    glm::vec3 ring,
    glm::vec3 core
) {
    Node* pivot = addGroup(
        root,
        position +
            glm::vec3(0.f, diameter * 0.5f, 0.f)
    );

    spinners.push_back(pivot);

    add(
        *pivot,
        sphere,
        {0.f, 0.f, 0.f},
        glm::vec3(diameter * 0.3f),
        core,
        core * 0.8f
    );

    for (int i = 0; i < 4; ++i) {
        Node* meridian = add(
            *pivot,
            torus,
            {0.f, 0.f, 0.f},
            glm::vec3(diameter),
            ring,
            ring * 0.5f
        );

        meridian->rotation = {
            PI * 0.5f,
            i * PI / 4.f,
            0.f
        };
    }

    for (float h : {-0.25f, 0.f, 0.25f}) {
        float d =
            diameter *
            std::sqrt(1.f - 4.f * h * h);

        add(
            *pivot,
            torus,
            {0.f, h * diameter, 0.f},
            glm::vec3(d),
            ring,
            ring * 0.5f
        );
    }

    add(
        *pivot,
        cylinder,
        {0.f, diameter * 0.5f, 0.f},
        {
            diameter * 0.16f,
            diameter * 0.1f,
            diameter * 0.16f
        },
        {0.7f, 0.55f, 0.2f}
    );
}

// =====================================================
// ARCHES AND BENCHES
// =====================================================

void ExpoScene::buildArches(
    float x0,
    float x1,
    float z
) {
    const float width = 4.5f;

    int count = static_cast<int>(
        (x1 - x0) / width
    );

    for (int i = 0; i < count; ++i) {
        float x = x0 + i * width;

        float height =
            3.2f +
            1.8f * std::fabs(
                std::sin(i * 1.7f + x0)
            );

        glm::vec3 a(x, 0.1f, z);

        glm::vec3 peak(
            x + width * 0.5f,
            height,
            z
        );

        glm::vec3 b(x + width, 0.1f, z);

        addRod(root, a, peak, 0.14f, snowWhite);
        addRod(root, peak, b, 0.14f, snowWhite);
    }
}

void ExpoScene::buildBench(glm::vec3 position) {
    Node* bench = addGroup(root, position);

    const glm::vec3 wood(0.35f, 0.20f, 0.12f);

    add(
        *bench,
        cube,
        {0.f, 1.1f, 0.f},
        {4.2f, 0.25f, 1.3f},
        wood
    );

    add(
        *bench,
        cube,
        {0.f, 1.6f, -0.65f},
        {4.2f, 1.2f, 0.2f},
        wood
    );

    for (float x : {-1.8f, 1.8f}) {
        add(
            *bench,
            cube,
            {x, 0.f, 0.f},
            {0.25f, 1.1f, 1.1f},
            wood
        );
    }
}

// =====================================================
// PEOPLE
// =====================================================

void ExpoScene::buildPerson(
    glm::vec3 position,
    float height,
    glm::vec3 coat,
    glm::vec3 hat,
    float facing
) {
    Node* person = addGroup(root, position);
    person->rotation.y = facing;

    add(
        *person,
        cylinder,
        {0.f, height * 0.35f, 0.f},
        {
            height * 0.30f,
            height * 0.50f,
            height * 0.23f
        },
        coat
    );

    for (float side : {-1.f, 1.f}) {
        add(
            *person,
            cylinder,
            {
                side * height * 0.09f,
                0.f,
                0.f
            },
            {
                height * 0.10f,
                height * 0.43f,
                height * 0.10f
            },
            {0.10f, 0.10f, 0.16f}
        );
    }

    add(
        *person,
        sphere,
        {0.f, height * 0.88f, 0.f},
        glm::vec3(height * 0.19f),
        {0.55f, 0.40f, 0.34f}
    );

    add(
        *person,
        sphere,
        {0.f, height * 0.96f, 0.f},
        {
            height * 0.23f,
            height * 0.15f,
            height * 0.23f
        },
        hat
    );
}

void ExpoScene::buildCrowd() {
    buildPerson(
        {-7.f, 0.f, 10.f},
        4.2f,
        {0.40f, 0.34f, 0.55f},
        {0.30f, 0.25f, 0.40f},
        0.4f
    );

    buildPerson(
        {-12.f, 0.f, 13.f},
        2.6f,
        {0.85f, 0.60f, 0.15f},
        {0.30f, 0.20f, 0.10f},
        -0.3f
    );

    buildPerson(
        {-10.f, 0.f, 14.f},
        2.7f,
        {0.70f, 0.20f, 0.55f},
        {0.45f, 0.10f, 0.35f},
        0.2f
    );

    buildPerson(
        {5.f, 0.f, 13.f},
        4.4f,
        {0.30f, 0.25f, 0.80f},
        {0.25f, 0.20f, 0.65f},
        -0.5f
    );

    buildPerson(
        {9.f, 0.f, 12.f},
        4.f,
        {0.90f, 0.15f, 0.10f},
        {0.80f, 0.10f, 0.08f},
        0.6f
    );
}

// =====================================================
// HIGH-RISE BUILDINGS
// =====================================================

void ExpoScene::buildDistrict() {
    const glm::vec3 wall(0.15f, 0.23f, 0.35f);
    const glm::vec3 roof(0.86f, 0.92f, 1.f);

    for (int side : {-1, 1}) {
        for (int i = 0; i < 5; ++i) {
            float x =
                side * (130.f + (i % 2) * 17.f);

            float z = 53.f - i * 40.f;
            float height = 34.f + (i % 4) * 7.f;

            Node* building = addGroup(
                root,
                {x, 0.f, z}
            );

            // Real blue_glass.png texture.
            Node* facade = add(
                *building,
                cube,
                {0.f, 0.f, 0.f},
                {23.f, height, 22.f},
                wall,
                {0.015f, 0.035f, 0.050f}
            );

            facade->texture = buildingGlass.get();
            facade->uvScale = {2.f, 5.f, 1.f};

            add(
                *building,
                cube,
                {0.f, height + 0.3f, 0.f},
                {25.f, 0.65f, 24.f},
                roof
            );

            for (int floor = 0; floor < 8; ++floor) {
                float y =
                    3.5f +
                    floor * (height - 5.f) / 8.f;

                glm::vec3 light =
                    (floor + i) % 3 == 0
                    ? blueLight
                    : warmLight;

                for (float face : {-1.f, 1.f}) {
                    add(
                        *building,
                        cube,
                        {0.f, y, face * 11.1f},
                        {18.5f, 0.85f, 0.12f},
                        light,
                        light * 0.40f
                    );
                }
            }

            for (float xx : {-3.5f, 3.5f}) {
                add(
                    *building,
                    cube,
                    {xx, height * 0.5f, 11.2f},
                    {0.65f, height - 2.f, 0.18f},
                    wall * 0.65f
                );
            }
        }
    }
}


 // =====================================================
 // TEXTURED WINTER TREES
 // =====================================================

void ExpoScene::buildWinterPark() {
    const glm::vec3 trunk(0.25f, 0.16f, 0.10f);

    for (int side : {-1, 1}) {
        for (int i = 0; i < 13; ++i) {
            float z = 78.f - i * 16.f;

            float x =
                side * (68.f + (i % 2) * 4.f);

            Node* tree = addGroup(
                root,
                {x, 0.f, z}
            );

            add(
                *tree,
                cylinder,
                {0.f, 0.f, 0.f},
                {0.65f, 2.2f, 0.65f},
                trunk
            );

            for (int layer = 0; layer < 3; ++layer) {
                float y = 1.f + layer * 2.5f;
                float radius =
                    4.5f - layer * 1.1f;

                Node* crown = add(
                    *tree,
                    cone,
                    {0.f, y, 0.f},
                    {
                        radius * 2.f,
                        5.5f,
                        radius * 2.f
                    },
                    {0.46f, 0.68f, 0.48f},
                    {0.008f, 0.025f, 0.015f}
                );

                crown->texture = pineTexture.get();
                crown->uvScale = {1.3f, 1.f, 1.f};
            }

            add(
                *tree,
                cone,
                {0.f, 7.9f, 0.f},
                {4.6f, 1.1f, 4.6f},
                snowWhite
            );
        }
    }

    for (int side : {-1, 1}) {
        for (int i = 0; i < 8; ++i) {
            float z = 70.f - i * 28.f;

            Node* lamp = addGroup(
                root,
                {side * 82.f, 0.f, z}
            );

            add(
                *lamp,
                cylinder,
                {0.f, 0.f, 0.f},
                {0.30f, 8.f, 0.30f},
                {0.20f, 0.25f, 0.31f}
            );

            add(
                *lamp,
                sphere,
                {0.f, 8.3f, 0.f},
                glm::vec3(1.3f),
                warmLight,
                warmLight * 0.8f
            );
        }
    }
}

// =====================================================
// EXPO ROADS, PAVILIONS AND RESIDENTIAL BLOCKS
// =====================================================

void ExpoScene::buildExpoLayout() {
    const glm::vec3 asphalt(0.10f, 0.15f, 0.21f);
    const glm::vec3 pavement(0.55f, 0.64f, 0.74f);
    const glm::vec3 garden(0.19f, 0.37f, 0.33f);

    const glm::vec3 center(6.f, 0.f, -42.f);

    constexpr int segments = 32;

    // Circular roads.
    for (int i = 0; i < segments; ++i) {
        float a = 2.f * PI * i / segments;
        float b = 2.f * PI * (i + 1) / segments;
        float mid = (a + b) * 0.5f;

        for (int ring = 0; ring < 2; ++ring) {
            float radius =
                ring == 0 ? 83.f : 98.f;

            float width =
                2.f * radius *
                std::sin((b - a) * 0.5f) +
                0.7f;

            Node* road = add(
                root,
                cube,
                {
                    center.x +
                        radius * std::cos(mid),
                    0.10f,
                    center.z +
                        radius * std::sin(mid)
                },
                {
                    width,
                    0.10f,
                    ring == 0 ? 7.5f : 12.f
                },
                ring == 0 ? pavement : asphalt
            );

            road->rotation.y =
                -mid - PI * 0.5f;
        }
    }

    // Radial paths and gardens.
    for (int i = 0; i < 12; ++i) {
        float angle = 2.f * PI * i / 12.f;
        float midRadius = 60.5f;

        Node* path = add(
            root,
            cube,
            {
                center.x +
                    midRadius * std::cos(angle),
                0.13f,
                center.z +
                    midRadius * std::sin(angle)
            },
            {35.f, 0.10f, 4.4f},
            pavement
        );

        path->rotation.y = -angle;

        float gardenAngle = angle + PI / 12.f;

        for (int j = 0; j < 2; ++j) {
            float radius = 53.f + j * 15.f;

            float x =
                center.x +
                radius * std::cos(gardenAngle);

            float z =
                center.z +
                radius * std::sin(gardenAngle);

            add(
                root,
                sphere,
                {x, 0.20f, z},
                {7.f, 0.55f, 5.f},
                garden
            );

            add(
                root,
                sphere,
                {x, 0.55f, z},
                {5.5f, 0.38f, 3.7f},
                snowWhite
            );
        }
    }

    // Small exhibition pavilions.
    for (int i = 0; i < 4; ++i) {
        float angle =
            PI * (0.25f + i * 0.5f);

        float x =
            center.x + 68.f * std::cos(angle);

        float z =
            center.z + 68.f * std::sin(angle);

        Node* pavilion = addGroup(
            root,
            {x, 0.f, z}
        );

        pavilion->rotation.y = -angle;

        // Blue glass facade.
        Node* smallFacade = add(
            *pavilion,
            cube,
            {0.f, 0.f, 0.f},
            {18.f, 7.f, 12.f},
            {0.17f, 0.30f, 0.42f}
        );

        smallFacade->texture = buildingGlass.get();
        smallFacade->uvScale = {2.f, 2.f, 1.f};

        add(
            *pavilion,
            cube,
            {0.f, 7.2f, 0.f},
            {20.f, 0.6f, 14.f},
            snowWhite
        );

        add(
            *pavilion,
            cube,
            {0.f, 3.f, 6.1f},
            {13.f, 3.5f, 0.12f},
            blueLight,
            blueLight * 0.15f
        );
    }

    // Northern residential streets.
    for (float x : {
        -130.f, -70.f, -10.f, 50.f, 110.f
    }) {
        add(
            root,
            cube,
            {x, 0.08f, -184.f},
            {12.f, 0.08f, 155.f},
            asphalt
        );
    }

    for (float z : {
        -127.f, -184.f, -243.f
    }) {
        add(
            root,
            cube,
            {-10.f, 0.08f, z},
            {285.f, 0.08f, 12.f},
            asphalt
        );
    }

    // Tall residential buildings.
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 4; ++col) {
            float x = -103.f + col * 60.f;
            float z = -156.f - row * 60.f;

            float height =
                36.f + ((row + col) % 3) * 8.f;

            Node* building = addGroup(
                root,
                {x, 0.f, z}
            );

            // Real brick.png texture.
            Node* facade = add(
                *building,
                cube,
                {0.f, 0.f, 0.f},
                {35.f, height, 26.f},
                {0.20f, 0.29f, 0.40f}
            );

            facade->texture = buildingBrick.get();
            facade->uvScale = {3.f, 5.f, 1.f};

            add(
                *building,
                cube,
                {0.f, height + 0.3f, 0.f},
                {37.f, 0.65f, 28.f},
                snowWhite
            );

            for (int floor = 0; floor < 8; ++floor) {
                float y =
                    3.5f +
                    floor * (height - 5.f) / 8.f;

                glm::vec3 light =
                    (floor + col + row) % 4 == 0
                    ? blueLight
                    : warmLight;

                for (float face : {-1.f, 1.f}) {
                    add(
                        *building,
                        cube,
                        {0.f, y, face * 13.1f},
                        {28.f, 0.85f, 0.12f},
                        light,
                        light * 0.40f
                    );
                }
            }

            // Two textured trees near each building.
            for (int side : {-1, 1}) {
                Node* tree = addGroup(
                    root,
                    {
                        x + side * 21.f,
                        0.f,
                        z + (col % 2 ? 8.f : -8.f)
                    }
                );

                add(
                    *tree,
                    cylinder,
                    {0.f, 0.f, 0.f},
                    {0.55f, 2.f, 0.55f},
                    {0.25f, 0.16f, 0.10f}
                );

                for (int layer = 0; layer < 2; ++layer) {
                    float radius =
                        4.f - layer * 1.2f;

                    Node* crown = add(
                        *tree,
                        cone,
                        {
                            0.f,
                            1.2f + layer * 2.8f,
                            0.f
                        },
                        {
                            radius * 2.f,
                            6.f,
                            radius * 2.f
                        },
                        {0.45f, 0.66f, 0.47f}
                    );

                    crown->texture = pineTexture.get();
                    crown->uvScale = {1.3f, 1.f, 1.f};
                }

                add(
                    *tree,
                    cone,
                    {0.f, 6.5f, 0.f},
                    {3.3f, 0.75f, 3.3f},
                    snowWhite
                );
            }
        }
    }
}

// =====================================================
// ANIMATION
// =====================================================

void ExpoScene::update(float time) {
    if (starPivot) {
        starPivot->rotation.y = time * 1.2f;
    }

    for (size_t i = 0; i < spinners.size(); ++i) {
        spinners[i]->rotation.y =
            time * (0.4f + 0.1f * i);
    }

    for (Twinkle& t : twinkles) {
        t.node->emissive =
            t.base *
            (
                0.70f +
                0.30f * std::sin(
                    time * 2.2f + t.phase
                )
            );
    }
}

// =====================================================
// DRAW
// =====================================================

void ExpoScene::draw(const Shader& shader) const {
    root.draw(
        shader,
        glm::mat4(1.f)
    );
}

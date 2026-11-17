
#include "ExpoScene.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "Primitives.h"

namespace {

constexpr float PI = 3.14159265358979323846f;

const glm::vec3 sphereCenter(6.f, 38.f, -42.f);
constexpr float sphereRadius = 30.f;

const glm::vec3 iceWhite(0.80f, 0.88f, 1.f);

float hash(float x) {
    float s = std::sin(x * 12.9898f) * 43758.5453f;
    return s - std::floor(s);
}

} // namespace

ExpoScene::ExpoScene()
    : plane(Primitives::plane()),
      cube(Primitives::cube()),
      cylinder(Primitives::cylinder()),
      taper(Primitives::cylinder(32, 0.25f)),
      cone(Primitives::cylinder(32, 0.f)),
      sphere(Primitives::sphere()),
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

    buildGround();
    buildPodium();
    buildNurAlem();
    buildTree();

    buildOrnament(
        {-2.f, 0.f, 14.f},
        5.5f,
        iceWhite,
        {1.f, 0.15f, 0.12f}
    );

    buildOrnament(
        {18.f, 0.f, 4.f},
        3.5f,
        {0.75f, 0.85f, 1.f},
        {0.55f, 0.35f, 1.f}
    );

    buildOrnament(
        {31.f, 0.f, 4.f},
        3.5f,
        {0.75f, 0.85f, 1.f},
        {0.55f, 0.35f, 1.f}
    );

    buildArches(-46.f, -26.f, -8.f);
    buildArches(6.f, 38.f, -8.f);

    buildBench({23.f, 0.f, 0.f});
    buildCrowd();
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
    glm::vec3 d = to - from;

    Node* rod = add(
        parent,
        cylinder,
        from,
        {thickness, glm::length(d), thickness},
        color,
        color
    );

    rod->rotation.z = -std::atan2(d.x, d.y);

    return rod;
}

void ExpoScene::buildGround() {
    add(
        root,
        plane,
        {0.f, -0.05f, 0.f},
        {700.f, 1.f, 700.f},
        {0.80f, 0.83f, 0.92f},
        {0.20f, 0.21f, 0.26f}
    );

    add(
        root,
        plane,
        {6.f, 0.02f, -10.f},
        {130.f, 1.f, 70.f},
        {0.88f, 0.90f, 0.96f},
        {0.30f, 0.31f, 0.36f}
    );

    for (int i = 0; i < 14; ++i) {
        float x = -60.f + hash(i * 3.1f) * 140.f;
        float z = -20.f + hash(i * 5.7f) * 45.f;

        add(
            root,
            plane,
            {x, 0.04f, z},
            {
                6.f + hash(i * 7.3f) * 10.f,
                1.f,
                3.f + hash(i * 2.9f) * 5.f
            },
            {0.72f, 0.75f, 0.86f},
            {0.14f, 0.15f, 0.19f}
        );
    }
}

void ExpoScene::buildPodium() {
    const glm::vec3 body(0.055f, 0.09f, 0.15f);
    const glm::vec3 frame(0.20f, 0.27f, 0.35f);

    add(
        root, cube,
        {6.f, 0.f, -42.f},
        {120.f, 10.f, 28.f},
        body,
        {0.012f, 0.025f, 0.045f}
    );

    add(
        root, cube,
        {6.f, 10.f, -41.f},
        {126.f, 0.55f, 34.f},
        {0.50f, 0.56f, 0.64f},
        {0.065f, 0.08f, 0.10f}
    );

    add(
        root, cube,
        {6.f, 0.45f, -26.8f},
        {119.f, 0.38f, 1.1f},
        {0.23f, 0.28f, 0.36f},
        {0.025f, 0.04f, 0.065f}
    );

    for (int column = 0; column < 30; ++column) {
        float x = -51.5f + column * 3.95f;

        for (int row = 0; row < 3; ++row) {
            float y = 1.15f + row * 2.55f;
            float z = -26.62f - (y - 1.15f) * 0.12f;

            float variation =
                0.88f + 0.04f * ((column + row * 3) % 4);

            Node* pane = add(
                root,
                cube,
                {x, y, z},
                {3.76f, 2.38f, 0.10f},
                glm::vec3(0.25f, 0.40f, 0.61f) * variation,
                {0.055f, 0.050f, 0.042f}
            );

            pane->texture = podiumGlass.get();
            pane->uvScale = {1.f, 1.f, 1.f};
            pane->rotation.x = -0.12f;

            if ((column + row) % 3 != 1) {
                float lightY = y + 1.75f;

                add(
                    root,
                    cube,
                    {
                        x,
                        lightY,
                        -26.44f - (lightY - 1.15f) * 0.12f
                    },
                    {2.65f, 0.045f, 0.035f},
                    {0.88f, 0.78f, 0.60f},
                    {0.30f, 0.20f, 0.10f}
                );
            }
        }
    }

    for (int i = 0; i <= 30; ++i) {
        float x = -53.48f + i * 3.95f;

        Node* mullion = add(
            root,
            cube,
            {x, 1.05f, -26.44f},
            {0.09f, 7.8f, 0.13f},
            frame,
            {0.025f, 0.04f, 0.06f}
        );

        mullion->rotation.x = -0.12f;
    }

    for (int row = 0; row <= 3; ++row) {
        float y = 1.05f + row * 2.55f;

        add(
            root,
            cube,
            {6.f, y, -26.42f - (y - 1.05f) * 0.12f},
            {119.f, 0.085f, 0.16f},
            frame,
            {0.025f, 0.04f, 0.06f}
        );
    }

    add(
        root,
        cube,
        {6.f, 8.9f, -27.35f},
        {119.f, 0.07f, 0.12f},
        {0.40f, 0.58f, 0.75f},
        {0.12f, 0.21f, 0.31f}
    );

    for (float x : {-6.f, 6.f, 18.f}) {
        add(
            root,
            cube,
            {x, 0.8f, -26.34f},
            {2.5f, 4.6f, 0.12f},
            {0.035f, 0.075f, 0.12f},
            {0.008f, 0.02f, 0.035f}
        );

        add(
            root,
            cube,
            {x, 0.8f, -26.23f},
            {0.065f, 4.6f, 0.07f},
            frame,
            {0.04f, 0.055f, 0.075f}
        );

        add(
            root,
            cube,
            {x, 5.48f, -26.14f},
            {2.7f, 0.07f, 0.08f},
            {0.90f, 0.80f, 0.63f},
            {0.44f, 0.30f, 0.15f}
        );
    }

    for (float x = 40.f; x <= 66.f; x += 1.6f) {
        add(
            root,
            sphere,
            {x, 10.65f, -24.5f},
            glm::vec3(0.25f),
            {0.70f, 0.82f, 0.96f},
            {0.28f, 0.40f, 0.56f}
        );
    }
}

void ExpoScene::buildNurAlem() {
    // Textured inner shell hides gaps between panels.
    Node* globe = add(
        root,
        bigSphere,
        sphereCenter,
        glm::vec3(sphereRadius * 2.f),
        {0.10f, 0.30f, 0.43f},
        {0.055f, 0.18f, 0.24f}
    );

    globe->texture = expoGlass.get();
    globe->uvScale = {3.f, 2.f, 1.f};

    // Smaller glass panels.
    constexpr int columns = 36;
    constexpr int rows = 20;

    for (int row = 0; row < rows; ++row) {
        float latitude =
            -1.48f +
            (row + 0.5f) * 2.96f / rows;

        float c = std::cos(latitude);

        float panelWidth =
            2.f * PI * sphereRadius * c /
            columns * 0.98f;

        float panelHeight =
            sphereRadius * 2.96f / rows * 0.98f;

        for (int column = 0; column < columns; ++column) {
            float longitude =
                2.f * PI * (column + 0.5f) / columns;

            glm::vec3 normal(
                c * std::sin(longitude),
                std::sin(latitude),
                c * std::cos(longitude)
            );

            float variation =
                0.90f +
                0.025f * ((row * 3 + column) % 5);

            Node* panel = add(
                root,
                cube,
                sphereCenter +
                    normal * (sphereRadius + 0.08f),
                {
                    panelWidth,
                    panelHeight,
                    0.10f
                },
                glm::vec3(
                    0.32f,
                    0.58f,
                    0.76f
                ) * variation,
                {
                    0.035f,
                    0.11f,
                    0.16f
                }
            );

            panel->texture = expoGlass.get();
            panel->uvScale = {1.f, 1.f, 1.f};
            panel->rotation.y = longitude;
            panel->rotation.x = -latitude;

            if ((column + row * 2) % 9 == 0) {
                panel->emissive = {
                    0.055f,
                    0.19f,
                    0.25f
                };
            }
        }
    }

    // Soft cyan illuminated rings.
    for (int i = 0; i < 5; ++i) {
        float y = -17.f + i * 8.5f;

        float crossSection =
            std::sqrt(
                std::max(
                    0.f,
                    sphereRadius * sphereRadius - y * y
                )
            );

        float ringRadius = crossSection * 0.96f;

        add(
            root,
            torus,
            sphereCenter + glm::vec3(0.f, y, 0.f),
            glm::vec3(ringRadius * 2.f),
            {0.10f, 0.56f, 0.70f},
            {0.06f, 0.24f, 0.32f}
        );
    }

    // Top cap.
    add(
        root,
        cylinder,
        sphereCenter + glm::vec3(0.f, 29.2f, 0.f),
        {3.2f, 1.f, 3.2f},
        {0.36f, 0.49f, 0.60f},
        {0.065f, 0.09f, 0.12f}
    );

    // Spire.
    add(
        root,
        taper,
        sphereCenter + glm::vec3(0.f, 30.f, 0.f),
        {1.2f, 12.f, 1.2f},
        {0.68f, 0.77f, 0.86f},
        {0.13f, 0.18f, 0.23f}
    );
}

void ExpoScene::buildTree() {
    Node* tree = addGroup(root, {-15.f, 0.f, -2.f});

    const glm::vec3 foliageGlow(
        0.025f, 0.055f, 0.018f
    );

    // Trunk.
    add(
        *tree,
        cylinder,
        {0.f, 0.f, 0.f},
        {1.5f, 3.2f, 1.5f},
        {0.25f, 0.16f, 0.10f}
    );

    // Textured evergreen foliage.
    constexpr int layers = 8;

    for (int i = 0; i < layers; ++i) {
        float base = 0.8f + i * 3.1f;
        float radius = 8.5f * (1.f - i / 9.f);
        float height = 7.3f - i * 0.22f;

        float shade = 0.95f + 0.04f * (i % 3);

        Node* crown = add(
            *tree,
            cone,
            {0.f, base, 0.f},
            {
                2.f * radius,
                height,
                2.f * radius
            },
            glm::vec3(
                0.62f * shade,
                0.78f * shade,
                0.56f * shade
            ),
            foliageGlow
        );

        crown->texture = pineTexture.get();
        crown->uvScale = {1.4f, 1.f, 1.f};

        for (int j = 0; j < 8; ++j) {
            float a =
                2.f * PI *
                (j + 0.5f * (i % 2)) / 8.f;

            float jitter =
                0.91f +
                0.14f * hash(i * 23.f + j * 7.f);

            float r = radius * 0.64f * jitter;

            Node* branch = add(
                *tree,
                cone,
                {
                    r * std::cos(a),
                    base + 0.1f +
                        0.25f * hash(j * 5.f + i),
                    r * std::sin(a)
                },
                {
                    radius * 0.95f,
                    height * 0.53f,
                    radius * 0.95f
                },
                {0.60f, 0.76f, 0.54f},
                foliageGlow
            );

            branch->texture = pineTexture.get();
            branch->uvScale = {1.f, 1.f, 1.f};
            branch->rotation.y = a;
        }

        for (int j = 0; j < 6; ++j) {
            if ((j + i) % 3 == 0) {
                continue;
            }

            float a =
                2.f * PI *
                (j + i * 0.31f) / 6.f;

            float r =
                radius *
                (0.82f +
                 0.09f * hash(j * 13.f + i));

            add(
                *tree,
                sphere,
                {
                    r * std::cos(a),
                    base + 0.35f,
                    r * std::sin(a)
                },
                {0.40f, 0.09f, 0.29f},
                {0.84f, 0.89f, 0.94f},
                {0.01f, 0.015f, 0.02f}
            );
        }
    }

    // Dense golden garland with colored accents.
    const glm::vec3 warm(1.f, 0.76f, 0.32f);
    const glm::vec3 cool(0.35f, 0.72f, 1.f);
    const glm::vec3 pink(1.f, 0.35f, 0.62f);

    constexpr int bulbs = 260;

    for (int k = 0; k < bulbs; ++k) {
        float t = k / float(bulbs - 1);

        float y = 1.9f + t * 25.4f;

        float r =
            8.35f * (1.f - t * 0.90f) +
            0.20f;

        float a =
            2.f * PI *
            (t * 8.0f + 0.15f);

        glm::vec3 p(
            r * std::cos(a),
            y,
            r * std::sin(a)
        );

        glm::vec3 color = warm;

        if (k % 17 == 0) {
            color = cool;
        } else if (k % 23 == 0) {
            color = pink;
        }

        Node* bulb = add(
            *tree,
            sphere,
            p,
            glm::vec3(0.34f),
            color,
            color * 0.85f
        );

        twinkles.push_back({
            bulb,
            bulb->emissive,
            hash(k * 1.37f) * 2.f * PI
        });
    }

    // Gold ornaments.
    for (int i = 0; i < 8; ++i) {
        float t = (i + 0.5f) / 8.f;
        float y = 2.2f + t * 24.f;
        float r = 8.f * (1.f - t * 0.9f);

        for (int j = 0; j < 3; ++j) {
            float a =
                2.f * PI * j / 3.f + i * 0.55f;

            add(
                *tree,
                sphere,
                {
                    r * std::cos(a),
                    y,
                    r * std::sin(a)
                },
                glm::vec3(0.30f),
                {0.82f, 0.62f, 0.22f},
                {0.14f, 0.09f, 0.025f}
            );
        }
    }

    // Golden star.
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
                (i % 2 == 0) ? 2.f : 1.35f,
                0.20f
            },
            {1.f, 0.86f, 0.42f},
            {0.48f, 0.32f, 0.10f}
        );

        ray->rotation.z = i * PI / 4.f;
    }
}

void ExpoScene::buildOrnament(
    glm::vec3 position,
    float diameter,
    glm::vec3 ring,
    glm::vec3 core
) {
    Node* pivot = addGroup(
        root,
        position + glm::vec3(
            0.f,
            diameter * 0.5f,
            0.f
        )
    );

    spinners.push_back(pivot);

    add(
        *pivot,
        sphere,
        {0.f, 0.f, 0.f},
        glm::vec3(diameter * 0.3f),
        core,
        core
    );

    for (int k = 0; k < 6; ++k) {
        Node* meridian = add(
            *pivot,
            torus,
            {0.f, 0.f, 0.f},
            glm::vec3(diameter),
            ring,
            ring
        );

        meridian->rotation = {
            PI * 0.5f,
            k * PI / 6.f,
            0.f
        };
    }

    for (
        float h :
        {-0.36f, -0.2f, 0.f, 0.2f, 0.36f}
    ) {
        float d =
            diameter *
            std::sqrt(1.f - 4.f * h * h);

        add(
            *pivot,
            torus,
            {0.f, h * diameter, 0.f},
            glm::vec3(d),
            ring,
            ring
        );
    }

    const glm::vec3 gold(0.7f, 0.55f, 0.2f);

    add(
        *pivot,
        cylinder,
        {
            0.f,
            diameter * 0.5f - 0.05f,
            0.f
        },
        {
            diameter * 0.16f,
            diameter * 0.1f,
            diameter * 0.16f
        },
        gold,
        gold * 0.3f
    );
}

void ExpoScene::buildArches(
    float x0,
    float x1,
    float z
) {
    const float w = 4.5f;
    int count = static_cast<int>((x1 - x0) / w);

    Node* row = addGroup(
        root,
        {0.f, 0.f, 0.f}
    );

    for (int i = 0; i < count; ++i) {
        float x = x0 + i * w;

        float h =
            3.2f +
            1.8f * std::fabs(
                std::sin(i * 1.7f + x0)
            );

        glm::vec3 a(x, 0.1f, z);
        glm::vec3 peak(x + w * 0.5f, h, z);
        glm::vec3 b(x + w, 0.1f, z);

        addRod(*row, a, peak, 0.14f, iceWhite);
        addRod(*row, peak, b, 0.14f, iceWhite);

        addRod(
            *row,
            {x + w * 0.25f, h * 0.5f, z},
            {x + w * 0.75f, h * 0.5f, z},
            0.1f,
            iceWhite
        );
    }

    addRod(
        *row,
        {x0, 0.1f, z},
        {x0 + count * w, 0.1f, z},
        0.14f,
        iceWhite
    );
}

void ExpoScene::buildBench(glm::vec3 position) {
    const glm::vec3 wood(0.35f, 0.20f, 0.12f);

    Node* bench = addGroup(root, position);

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

    add(
        *bench,
        cube,
        {-1.8f, 0.f, 0.f},
        {0.25f, 1.1f, 1.1f},
        wood * 0.7f
    );

    add(
        *bench,
        cube,
        {1.8f, 0.f, 0.f},
        {0.25f, 1.1f, 1.1f},
        wood * 0.7f
    );
}

void ExpoScene::buildPerson(
    glm::vec3 position,
    float height,
    glm::vec3 coat,
    glm::vec3 hat,
    float facing
) {
    const float h = height;

    const glm::vec3 pants(0.10f, 0.10f, 0.16f);
    const glm::vec3 skin(0.55f, 0.40f, 0.34f);

    Node* g = addGroup(root, position);
    g->rotation.y = facing;

    for (float s : {-1.f, 1.f}) {
        add(
            *g,
            cylinder,
            {s * 0.07f * h, 0.f, 0.f},
            {
                0.1f * h,
                0.42f * h,
                0.1f * h
            },
            pants
        );

        add(
            *g,
            cylinder,
            {s * 0.17f * h, 0.5f * h, 0.f},
            {
                0.075f * h,
                0.3f * h,
                0.075f * h
            },
            coat,
            coat * 0.22f
        );
    }

    add(
        *g,
        cylinder,
        {0.f, 0.36f * h, 0.f},
        {
            0.3f * h,
            0.44f * h,
            0.22f * h
        },
        coat,
        coat * 0.22f
    );

    add(
        *g,
        sphere,
        {0.f, 0.88f * h, 0.f},
        glm::vec3(0.19f * h),
        skin
    );

    add(
        *g,
        sphere,
        {0.f, 0.93f * h, 0.f},
        {
            0.23f * h,
            0.15f * h,
            0.23f * h
        },
        hat,
        hat * 0.22f
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
        4.0f,
        {0.90f, 0.15f, 0.10f},
        {0.80f, 0.10f, 0.08f},
        0.6f
    );

    buildPerson(
        {11.5f, 0.f, 20.f},
        2.8f,
        {0.80f, 0.10f, 0.25f},
        {0.70f, 0.08f, 0.20f},
        3.0f
    );

    add(
        root,
        cylinder,
        {3.2f, 0.f, 16.f},
        {1.8f, 0.3f, 1.8f},
        {0.8f, 0.1f, 0.08f},
        {0.2f, 0.02f, 0.02f}
    );
}

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

void ExpoScene::draw(const Shader& shader) const {
    root.draw(
        shader,
        glm::mat4(1.f)
    );
}

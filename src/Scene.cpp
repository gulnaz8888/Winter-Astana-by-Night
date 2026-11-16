
#include "Scene.h"
#include "Primitives.h"
#include <cmath>

namespace {

constexpr float PI = 3.14159265359f;

const glm::vec3 snowColor(0.78f, 0.85f, 0.96f);
const glm::vec3 snowGlow(0.16f, 0.19f, 0.25f);
const glm::vec3 white(0.84f, 0.89f, 0.96f);
const glm::vec3 metal(0.075f, 0.10f, 0.15f);
const glm::vec3 warm(1.f, 0.85f, 0.52f);
const glm::vec3 warmGlow(1.f, 0.73f, 0.33f);

// Fluffy snowdrifts
void fluffyMound(
    SceneGeometry& g,
    float x,
    float z,
    float radius,
    float height,
    int seed
) {
    g.dome({x, 0.015f, z}, radius, height, 8, 24);

    for (int i = 0; i < 6; ++i) {
        float a = 2.f * PI * i / 6.f + seed * 0.37f;
        float d = radius * (0.35f + (i % 3) * 0.08f);

        float px = x + std::cos(a) * d;
        float pz = z + std::sin(a) * d;

        g.dome(
            {px, 0.018f, pz},
            radius * (0.44f + (i % 3) * 0.07f),
            height * (0.48f + (i % 2) * 0.13f),
            6, 18
        );
    }
}

// Snow patches on conifer branches
void fluffyTreeSnow(
    SceneGeometry& g,
    float x,
    float z,
    float base,
    float radius,
    float tierHeight,
    int seed
) {
    for (int band = 0; band < 3; ++band) {
        float fraction = 0.18f + band * 0.23f;
        float y = base + tierHeight * fraction;
        float surfaceRadius = radius * (1.f - fraction);

        int count = 12 - band * 2;

        for (int k = 0; k < count; ++k) {
            if ((k + seed + band * 2) % 5 == 0)
                continue;

            float a =
                2.f * PI * k / count +
                seed * 0.19f +
                band * 0.23f;

            float variation =
                0.90f + ((k + seed) % 4) * 0.055f;

            float r = surfaceRadius * variation;

            float px = x + r * std::cos(a);
            float pz = z + r * std::sin(a);

            float size =
                (0.32f + (k % 3) * 0.075f) *
                (1.f - band * 0.12f);

            g.dome(
                {px, y, pz},
                size * 1.15f,
                size * 0.52f,
                5, 12
            );

            g.dome(
                {
                    px + std::cos(a) * size * 0.25f,
                    y + 0.025f,
                    pz + std::sin(a) * size * 0.25f
                },
                size * 0.75f,
                size * 0.37f,
                4, 10
            );
        }
    }

    g.dome(
        {x, base + tierHeight * 0.94f, z},
        0.20f, 0.18f, 5, 12
    );
}

} // namespace

Scene::Scene()
    : sphere(Primitives::sphere(32, 48)) {

    snowTexture = std::make_unique<Texture>(
        "assets/textures/snow.png"
    );
    asphaltTexture = std::make_unique<Texture>(
        "assets/textures/asphalt.png"
    );
    brickTexture = std::make_unique<Texture>(
        "assets/textures/brick.png"
    );
    goldGlassTexture = std::make_unique<Texture>(
        "assets/textures/gold_glass.png"
    );
    silverMetalTexture = std::make_unique<Texture>(
        "assets/textures/silver_metal.png"
    );
    blueGlassTexture = std::make_unique<Texture>(
        "assets/textures/blue_glass.png"
    );
    lampGlassTexture = std::make_unique<Texture>(
        "assets/textures/lamp_glass.png"
    );
    pineTexture = std::make_unique<Texture>(
        "assets/textures/pine_needles.png"
    );

    buildGround();
    buildAvenue();
    buildLamps();
    buildSkyline();
    buildMosque();
    buildKhanShatyr();
    buildBaiterek();
    buildExtraCity();
}

Node* Scene::attach(
    Node& parent,
    const SceneGeometry& geometry,
    glm::vec3 color,
    glm::vec3 emission,
    const Texture* texture
) {
    if (geometry.indices.empty())
        return nullptr;

    meshes.push_back(
        std::make_unique<Mesh>(
            geometry.vertices,
            geometry.indices
        )
    );

    auto node = std::make_unique<Node>();

    node->mesh = meshes.back().get();
    node->color = color;
    node->emissive = emission;
    node->texture = texture;

    return parent.addChild(std::move(node));
}

// =====================================================
// GROUND
// =====================================================

void Scene::buildGround() {
    SceneGeometry ground, path, borders, stone, snowdrifts;

    ground.box(
        {0, -0.38f, -30},
        {240, 0.3f, 310}
    );

    path.box(
        {0, -0.02f, -5},
        {16, 0.06f, 110}
    );

    for (float side : {-1.f, 1.f}) {
        borders.box(
            {side * 8.7f, -0.02f, -5},
            {1.1f, 0.28f, 110}
        );

        stone.box(
            {side * 9.f, -0.08f, -5},
            {1.8f, 0.2f, 110}
        );

        for (int i = 0; i < 6; ++i) {
            float z = 29.f - i * 14.f;

            stone.box(
                {side * 9.f, 0.15f, z},
                {0.35f, 0.45f, 3.6f}
            );
        }

        for (int i = 0; i < 15; ++i) {
            float z = 41.f - i * 6.5f;
            float x = side * (10.2f + (i % 3) * 0.35f);

            fluffyMound(
                snowdrifts, x, z,
                0.9f + (i % 4) * 0.18f,
                0.24f + (i % 3) * 0.08f,
                i
            );
        }

        for (int i = 0; i < 10; ++i) {
            float z = 39.f - i * 9.2f;
            float x = side * (19.f + (i % 3) * 1.6f);

            fluffyMound(
                snowdrifts, x, z,
                2.0f + (i % 3) * 0.35f,
                0.38f + (i % 4) * 0.10f,
                i + 30
            );
        }
    }

    for (int i = 0; i < 4; ++i) {
        stone.box(
            {0, 0.12f + i * 0.17f, -49.f - i * 1.2f},
            {21.f - i * 1.2f, 0.17f, 2.f}
        );
    }

    ground.frustum(
        {0, 0.25f, -60},
        10.5f, 10.5f, 0.4f, 48
    );

    for (int i = 0; i < 12; ++i) {
        float a = 2.f * PI * i / 12.f;
        float r = 12.f + (i % 3) * 0.7f;

        fluffyMound(
            snowdrifts,
            r * std::cos(a),
            -60.f + r * std::sin(a),
            0.8f, 0.22f, i + 60
        );
    }

    attach(
        root, ground,
        snowColor, snowGlow,
        snowTexture.get()
    );

    attach(
        root, path,
        {0.70f, 0.77f, 0.88f},
        {0.16f, 0.19f, 0.24f},
        asphaltTexture.get()
    );

    attach(root, borders, snowColor, snowGlow);

    attach(
        root, stone,
        {0.29f, 0.34f, 0.43f},
        {0.045f, 0.06f, 0.085f}
    );

    attach(
        root, snowdrifts,
        {0.96f, 0.97f, 1.f},
        {0.18f, 0.21f, 0.27f},
        snowTexture.get()
    );
}

// =====================================================
// TREES AND IMPROVED BENCHES
// =====================================================

void Scene::buildAvenue() {
    SceneGeometry green, treeSnow, trunks;
    SceneGeometry wood, iron, benchSnow, groundSnow;
    SceneGeometry benchGlow;

    // Trees
    for (int i = 0; i < 7; ++i) {
        float z = 34.f - i * 13.f;

        for (float side : {-1.f, 1.f}) {
            float x = side * (
                13.8f + (i % 2) * 1.3f
            );

            float h = 7.7f + (i % 3) * 0.9f;

            trunks.frustum(
                {x, 0, z},
                0.28f, 0.18f, 2.6f, 8
            );

            for (int tier = 0; tier < 4; ++tier) {
                float base = 1.0f + tier * h * 0.17f;
                float r = 3.2f * (1.f - tier * 0.19f);
                float th = h * 0.47f;

                green.frustum(
                    {x, base, z},
                    r, 0, th, 12
                );

                fluffyTreeSnow(
                    treeSnow,
                    x, z, base, r, th,
                    i * 4 + tier + (side > 0 ? 7 : 0)
                );
            }

            fluffyMound(
                groundSnow, x, z,
                2.6f, 0.34f,
                i + (side > 0 ? 15 : 25)
            );
        }
    }

    // Improved benches
    for (int i = 0; i < 5; ++i) {
        float z = 26.f - i * 15.f;

        for (float side : {-1.f, 1.f}) {
            float x = side * 10.7f;
            float backX = x + side * 0.66f;

            // Five wooden seat slats
            for (int plank = 0; plank < 5; ++plank) {
                float seatX =
                    x - side * 0.43f +
                    side * plank * 0.21f;

                wood.box(
                    {seatX, 0.79f, z},
                    {0.17f, 0.12f, 3.32f}
                );
            }

            // Four wooden backrest slats
            for (int plank = 0; plank < 4; ++plank) {
                float y = 1.04f + plank * 0.21f;

                wood.box(
                    {backX, y, z},
                    {0.14f, 0.16f, 3.30f}
                );
            }

            // Metal legs and armrests
            for (float dz : {-1.23f, 1.23f}) {
                iron.box(
                    {x - side * 0.35f, 0.40f, z + dz},
                    {0.14f, 0.80f, 0.14f}
                );

                iron.box(
                    {backX, 0.48f, z + dz},
                    {0.14f, 0.96f, 0.14f}
                );

                iron.box(
                    {x + side * 0.08f, 1.02f, z + dz},
                    {1.14f, 0.10f, 0.14f}
                );

                iron.box(
                    {x, 0.48f, z + dz},
                    {1.04f, 0.10f, 0.12f}
                );
            }

            // Fluffy snow on seats and backrests
            for (int k = 0; k < 7; ++k) {
                float zz = z - 1.43f + k * 0.47f;

                float xx = x + (
                    k % 2 == 0
                    ? -side * 0.12f
                    : side * 0.08f
                );

                benchSnow.dome(
                    {xx, 0.87f, zz},
                    0.30f + (k % 3) * 0.065f,
                    0.12f + (k % 2) * 0.045f,
                    6, 16
                );

                benchSnow.dome(
                    {backX, 1.76f, zz},
                    0.22f + (k % 2) * 0.04f,
                    0.10f + (k % 3) * 0.025f,
                    5, 14
                );
            }

            // Warm decorative strip under each seat
            benchGlow.box(
                {x, 0.68f, z},
                {0.055f, 0.035f, 2.85f}
            );
        }
    }

    attach(
        root, green,
        {0.70f, 0.90f, 0.72f},
        {0.025f, 0.045f, 0.025f},
        pineTexture.get()
    );

    attach(
        root, treeSnow,
        {0.98f, 0.99f, 1.f},
        {0.22f, 0.24f, 0.28f},
        snowTexture.get()
    );

    attach(
        root, groundSnow,
        snowColor, snowGlow,
        snowTexture.get()
    );

    attach(
        root, trunks,
        {0.21f, 0.14f, 0.10f},
        {0.025f, 0.02f, 0.02f}
    );

    attach(
        root, wood,
        {0.65f, 0.49f, 0.35f},
        {0.05f, 0.027f, 0.015f},
        brickTexture.get()
    );

    attach(
        root, iron,
        metal,
        {0.012f, 0.02f, 0.03f},
        silverMetalTexture.get()
    );

    attach(
        root, benchSnow,
        snowColor, snowGlow,
        snowTexture.get()
    );

    attach(
        root, benchGlow,
        {1.f, 0.72f, 0.30f},
        {0.40f, 0.23f, 0.065f}
    );
}

// =====================================================
// LAMPS AND GARLANDS
// =====================================================

void Scene::buildLamps() {
    SceneGeometry posts, hoods, bulbs, cables, snow;

    for (int i = 0; i < 7; ++i) {
        float z = 35.f - i * 13.f;

        for (float side : {-1.f, 1.f}) {
            float x = side * 8.5f;

            posts.frustum(
                {x, 0, z},
                0.20f, 0.11f, 7.4f, 10
            );

            posts.frustum(
                {x, 0, z},
                0.43f, 0.25f, 0.45f, 10
            );

            hoods.dome(
                {x, 7.45f, z},
                0.67f, 0.23f, 4, 16
            );

            bulbs.frustum(
                {x, 7.3f, z},
                0.49f, 0.49f, 0.15f, 16
            );

            snow.dome(
                {x, 7.63f, z},
                0.56f, 0.15f, 3, 12
            );
        }

        for (int k = 0; k < 32; ++k) {
            float t = k / 32.f;
            float u = (k + 1) / 32.f;

            auto point = [&](float f) {
                return glm::vec3(
                    -8.5f + 17.f * f,
                    7.1f - 1.35f * std::sin(PI * f),
                    z
                );
            };

            cables.beam(
                point(t), point(u), 0.025f, 4
            );
        }

        for (int k = 1; k < 20; ++k) {
            float t = k / 20.f;

            glm::vec3 p(
                -8.5f + 17.f * t,
                7.1f - 1.35f * std::sin(PI * t),
                z
            );

            cables.beam(
                p,
                p - glm::vec3(0, 0.17f, 0),
                0.018f, 4
            );

            bulbs.ball(
                p - glm::vec3(0, 0.25f, 0),
                0.12f
            );
        }
    }

    attach(
        root, posts,
        metal,
        {0.018f, 0.025f, 0.04f},
        silverMetalTexture.get()
    );

    attach(
        root, hoods,
        {0.25f, 0.29f, 0.36f},
        {0.045f, 0.055f, 0.075f}
    );

    attach(
        root, bulbs,
        warm, warmGlow,
        lampGlassTexture.get()
    );

    attach(
        root, cables,
        metal,
        {0.01f, 0.015f, 0.025f}
    );

    attach(root, snow, snowColor, snowGlow);
}

// =====================================================
// BUILDINGS
// =====================================================

void Scene::buildSkyline() {
    SceneGeometry gold, frames, glass, light, snow, buildings;

    for (float side : {-1.f, 1.f}) {
        float x = side * 18.5f;
        float z = -103.f;

        gold.frustum(
            {x, 0, z},
            5.2f, 3.45f, 31.f, 32
        );

        frames.frustum(
            {x, 31, z},
            3.5f, 3.2f, 1.f, 32
        );

        frames.frustum(
            {x, 32, z},
            2.7f, 0, 6.f, 16
        );

        for (int i = 0; i < 16; ++i) {
            float a = 2.f * PI * i / 16.f;

            frames.beam(
                {
                    x + 5.23f * std::sin(a),
                    0,
                    z + 5.23f * std::cos(a)
                },
                {
                    x + 3.48f * std::sin(a),
                    31,
                    z + 3.48f * std::cos(a)
                },
                0.045f, 4
            );
        }

        for (int j = 1; j < 16; ++j) {
            float y = j * 1.9f;
            float r = 5.2f - (y / 31.f) * 1.75f;

            frames.ring(
                {x, y, z},
                r + 0.035f, 0.025f, 32
            );

            float rr = r + 0.065f;

            glass.box(
                {x, y, z + rr},
                {1.7f, 1.5f, 0.07f}
            );
        }

        light.beam(
            {x, 37.5f, z},
            {x, 39.f, z},
            0.06f
        );
    }

    for (float side : {-1.f, 1.f}) {
        for (int i = 0; i < 1; ++i) {
            float x = side * (42.f + i * 12.f);
            float z = 3.f - i * 26.f;

            float w = 20.f + i * 2.f;
            float h = 11.f + (i % 3) * 3.2f;
            float d = 16.f;

            buildings.box(
                {x, 0, z},
                {w, h, d}
            );

            buildings.box(
                {x, h, z},
                {w + 0.6f, 0.45f, d + 0.6f}
            );

            snow.box(
                {x, h + 0.45f, z},
                {w + 0.9f, 0.25f, d + 0.9f}
            );

            for (
                float y = 2.5f;
                y < h - 1.f;
                y += 2.9f
            ) {
                for (int k = 0; k < 7; ++k) {
                    float xx =
                        x - w * 0.42f + k * w * 0.14f;

                    if (
                        (i + k + static_cast<int>(y)) % 4 == 0
                    ) {
                        continue;
                    }

                    light.box(
                        {xx, y, z + d * 0.5f + 0.06f},
                        {1.1f, 1.6f, 0.08f}
                    );
                }

                for (int k = 0; k < 5; ++k) {
                    float zz =
                        z - d * 0.4f + k * d * 0.2f;

                    light.box(
                        {
                            x - side * (w * 0.5f + 0.06f),
                            y,
                            zz
                        },
                        {0.08f, 1.6f, 1.f}
                    );
                }
            }

            for (float offset : {-0.45f, 0.f, 0.45f}) {
                frames.box(
                    {
                        x + w * offset,
                        0,
                        z + d * 0.5f + 0.12f
                    },
                    {0.22f, h, 0.25f}
                );
            }
        }
    }

    attach(
        root, gold,
        {0.94f, 0.68f, 0.18f},
        {0.24f, 0.14f, 0.025f},
        goldGlassTexture.get()
    );

    attach(
        root, frames,
        {0.49f, 0.45f, 0.30f},
        {0.08f, 0.07f, 0.045f},
        silverMetalTexture.get()
    );

    attach(
        root, glass,
        {0.40f, 0.60f, 0.76f},
        {0.018f, 0.045f, 0.07f},
        blueGlassTexture.get()
    );

    attach(
        root, light,
        warm,
        {0.62f, 0.37f, 0.12f}
    );

    attach(root, snow, snowColor, snowGlow);

    attach(
        root, buildings,
        {0.75f, 0.80f, 0.87f},
        {0.07f, 0.09f, 0.12f},
        brickTexture.get()
    );
}

// =====================================================
// MOSQUE
// =====================================================

void Scene::buildMosque() {
    auto group = std::make_unique<Node>();
    group->position = {80, 0, -105};

    Node* mosque = root.addChild(std::move(group));

    SceneGeometry walls, domes, details, windows;

    walls.box({0, 0, 0}, {28, 8, 21});
    walls.box({0, 8, 0}, {17, 2.5f, 15});

    walls.frustum(
        {0, 10.5f, 0},
        6.5f, 6.5f, 2.f, 32
    );

    domes.dome(
        {0, 12.5f, 0},
        7.3f, 7.f, 12, 40
    );

    details.beam(
        {0, 19.5f, 0},
        {0, 22, 0},
        0.10f
    );

    details.ring(
        {0, 12.7f, 0},
        7.35f, 0.10f, 48
    );

    for (float x : {-9.f, 9.f}) {
        for (float z : {-6.f, 6.f}) {
            domes.dome(
                {x, 8, z},
                3.4f, 3.3f, 8, 24
            );
        }
    }

    for (float sx : {-1.f, 1.f}) {
        for (float sz : {-1.f, 1.f}) {
            glm::vec3 p(
                sx * 16.f,
                0,
                sz * 11.f
            );

            walls.frustum(
                p, 1.1f, 0.68f, 26.f, 12
            );

            walls.frustum(
                p + glm::vec3(0, 20, 0),
                1.5f, 1.5f, 0.6f, 16
            );

            walls.frustum(
                p + glm::vec3(0, 25, 0),
                1.2f, 0.9f, 1.f, 16
            );

            domes.frustum(
                p + glm::vec3(0, 26, 0),
                1.05f, 0, 4.f, 16
            );

            details.beam(
                p + glm::vec3(0, 30, 0),
                p + glm::vec3(0, 32, 0),
                0.07f
            );
        }
    }

    for (int i = -5; i <= 5; ++i) {
        if (i == 0)
            continue;

        windows.box(
            {i * 2.2f, 2.1f, 10.57f},
            {1.1f, 3.3f, 0.08f}
        );

        domes.dome(
            {i * 2.2f, 5.4f, 10.60f},
            0.55f, 0.6f, 4, 12
        );
    }

    windows.box(
        {0, 0.5f, 10.65f},
        {3, 5, 0.1f}
    );

    attach(
        *mosque, walls,
        white,
        {0.23f, 0.24f, 0.27f},
        brickTexture.get()
    );

    attach(
        *mosque, domes,
        {0.75f, 0.82f, 0.88f},
        {0.15f, 0.19f, 0.23f}
    );

    attach(
        *mosque, details,
        {0.85f, 0.68f, 0.28f},
        {0.20f, 0.14f, 0.04f}
    );

    attach(
        *mosque, windows,
        warm,
        {0.60f, 0.35f, 0.12f}
    );

    for (auto& child : mosque->children) {
        child->scale.x = 1.18f;
    }
}

// =====================================================
// KHAN SHATYR
// =====================================================

void Scene::buildKhanShatyr() {
    auto group = std::make_unique<Node>();
    group->position = {-68, 0, -104};

    Node* tent = root.addChild(std::move(group));

    SceneGeometry fabric, seams, base, windows;
    glm::vec3 peak(3.5f, 31, -2);

    for (int i = 0; i < 48; ++i) {
        float a = 2.f * PI * i / 48.f;
        float b = 2.f * PI * (i + 1) / 48.f;

        glm::vec3 p(
            19 * std::cos(a),
            2,
            14 * std::sin(a)
        );

        glm::vec3 q(
            19 * std::cos(b),
            2,
            14 * std::sin(b)
        );

        fabric.triangle(p, peak, q);

        if (i % 2 == 0) {
            seams.beam(
                p, peak, 0.045f, 4
            );
        }
    }

    base.frustum(
        {0, 0, 0},
        18, 18, 2.2f, 48
    );

    seams.beam(
        peak,
        peak + glm::vec3(0.6f, 6, 0),
        0.15f
    );

    for (int i = 0; i < 12; ++i) {
        float a = PI * i / 12.f;

        windows.box(
            {
                16 * std::cos(a),
                0.6f,
                13 * std::sin(a) + 0.2f
            },
            {1.2f, 1.1f, 0.3f}
        );
    }

    attach(
        *tent, fabric,
        {0.73f, 0.80f, 0.90f},
        {0.14f, 0.18f, 0.24f}
    );

    attach(
        *tent, seams,
        {0.93f, 0.95f, 1.f},
        {0.23f, 0.26f, 0.32f}
    );

    attach(
        *tent, base,
        {0.68f, 0.73f, 0.83f},
        {0.065f, 0.08f, 0.115f},
        silverMetalTexture.get()
    );

    attach(
        *tent, windows,
        warm,
        {0.70f, 0.40f, 0.13f}
    );

    for (auto& child : tent->children) {
        child->scale.x = 1.15f;
    }
}

// =====================================================
// BAITEREK — TEAM LIGHTING PRESERVED
// =====================================================

void Scene::buildBaiterek() {
    auto group = std::make_unique<Node>();
    group->position = {0, 0.65f, -60};

    // Required for architectural lighting.
    group->isBaiterek = true;

    Node* monument = root.addChild(std::move(group));

    SceneGeometry ribs, lattice, shaft;
    SceneGeometry base, collar, panes;

    const float heights[] = {
        0, 10, 20, 26, 31, 36, 41, 47
    };

    const float radii[] = {
        3.35f, 2.65f, 2.15f, 2.5f,
        3.6f, 5.25f, 7.25f, 9.15f
    };

    constexpr int count = 24;

    auto point = [&](int level, int rib) {
        float a = 2.f * PI * rib / count;

        return glm::vec3(
            radii[level] * std::sin(a),
            heights[level],
            radii[level] * std::cos(a)
        );
    };

    for (int i = 0; i < count; ++i) {
        for (int j = 0; j < 7; ++j) {
            ribs.beam(
                point(j, i),
                point(j + 1, i),
                j < 3 ? 0.105f : 0.085f
            );

            if (j >= 2) {
                lattice.beam(
                    point(j, i),
                    point(j + 1, i + 1),
                    0.038f, 4
                );

                lattice.beam(
                    point(j, i + 1),
                    point(j + 1, i),
                    0.038f, 4
                );
            }
        }
    }

    for (int j = 1; j < 7; ++j) {
        lattice.ring(
            {0, heights[j], 0},
            radii[j], 0.07f, 72
        );
    }

    shaft.frustum(
        {0, 0, 0},
        1.35f, 1.35f, 40.f, 32
    );

    for (int i = 0; i < 8; ++i) {
        float a = 2.f * PI * i / 8.f;

        ribs.beam(
            {
                1.40f * std::sin(a),
                0,
                1.40f * std::cos(a)
            },
            {
                1.40f * std::sin(a),
                39,
                1.40f * std::cos(a)
            },
            0.045f, 4
        );
    }

    base.frustum(
        {0, 0, 0},
        5.7f, 5.7f, 0.5f, 48
    );

    base.frustum(
        {0, 0.5f, 0},
        4.7f, 4.7f, 0.3f, 48
    );

    collar.frustum(
        {0, 39.3f, 0},
        4.8f, 5.6f, 0.9f, 48
    );

    ribs.ring(
        {0, 40.2f, 0},
        5.7f, 0.12f, 72
    );

    for (int i = 0; i < 36; ++i) {
        float a = 2.f * PI * i / 36.f;

        ribs.beam(
            {
                5.7f * std::sin(a),
                40.1f,
                5.7f * std::cos(a)
            },
            {
                5.7f * std::sin(a),
                41.1f,
                5.7f * std::cos(a)
            },
            0.028f, 4
        );
    }

    ribs.ring(
        {0, 41.1f, 0},
        5.7f, 0.045f, 72
    );

    attach(
        *monument, ribs,
        white,
        {0.29f, 0.32f, 0.37f},
        silverMetalTexture.get()
    );

    attach(
        *monument, lattice,
        {0.61f, 0.69f, 0.78f},
        {0.15f, 0.19f, 0.24f},
        silverMetalTexture.get()
    );

    attach(
        *monument, shaft,
        {0.055f, 0.17f, 0.21f},
        {0.018f, 0.055f, 0.07f}
    );

    attach(
        *monument, base,
        {0.55f, 0.59f, 0.65f},
        {0.10f, 0.12f, 0.16f},
        silverMetalTexture.get()
    );

    attach(
        *monument, collar,
        {0.24f, 0.26f, 0.28f},
        {0.065f, 0.07f, 0.08f}
    );

    auto orb = std::make_unique<Node>();

    orb->mesh = &sphere;
    orb->position = {0, 45.7f, 0};
    orb->scale = {12.4f, 12.4f, 12.4f};

    orb->color = {1.f, 0.73f, 0.20f};
    orb->emissive = {0.46f, 0.28f, 0.045f};
    orb->texture = goldGlassTexture.get();

    monument->addChild(std::move(orb));

    float radius = 6.24f;

    for (int j = -6; j <= 6; ++j) {
        float theta = j * PI / 16.f;

        panes.ring(
            {
                0,
                45.7f + radius * std::sin(theta),
                0
            },
            radius * std::cos(theta),
            0.022f, 64
        );
    }

    for (int i = 0; i < 24; ++i) {
        float a = 2.f * PI * i / 24.f;

        for (int j = 0; j < 24; ++j) {
            float t0 =
                -PI * 0.5f + PI * j / 24.f;
            float t1 =
                -PI * 0.5f + PI * (j + 1) / 24.f;

            auto p = [&](float t) {
                return glm::vec3(
                    radius * std::cos(t) * std::sin(a),
                    45.7f + radius * std::sin(t),
                    radius * std::cos(t) * std::cos(a)
                );
            };

            panes.beam(
                p(t0), p(t1),
                0.022f, 4
            );
        }
    }

    panes.beam(
        {0, 51.9f, 0},
        {0, 54.2f, 0},
        0.055f
    );

    attach(
        *monument, panes,
        {0.31f, 0.27f, 0.14f},
        {0.045f, 0.038f, 0.018f}
    );

    auto pivot = std::make_unique<Node>();
    pivot->position = {0, 40.6f, 0};

    beaconPivot = monument->addChild(
        std::move(pivot)
    );

    SceneGeometry beacon;
    beacon.ball(
        {5.85f, 0, 0},
        0.12f
    );

    attach(
        *beaconPivot, beacon,
        warm, warmGlow
    );
}

// =====================================================
// UPDATE AND DRAW
// =====================================================


void Scene::buildExtraCity() {

    // =====================================
    // EXTRA CITY: 12 BACKGROUND BUILDINGS
    // =====================================

    struct BuildingInfo {
        float x;
        float z;
        float width;
        float height;
        float depth;
        bool office;
    };




    const BuildingInfo buildings[12] = {

        // FRONT ROW - small houses near the avenue
        // Positioned on the empty areas beside the roads
        {-35.f,  10.f, 14.f, 10.f, 13.f, false},
        {-65.f,  10.f, 16.f, 12.f, 14.f, false},
        { 35.f,  10.f, 14.f, 10.f, 13.f, false},
        { 65.f,  10.f, 16.f, 12.f, 14.f, false},

        // MIDDLE ROW - shifted outward to keep landmarks visible
        {-64.f, -32.f, 16.f, 16.f, 15.f, false},
        {-92.f, -32.f, 17.f, 19.f, 15.f, true },
        { 64.f, -32.f, 16.f, 16.f, 15.f, false},
        { 92.f, -32.f, 17.f, 19.f, 15.f, true },

        // BACK ROW - behind Khan Shatyr and the mosque
        // These cannot obstruct the landmarks from the front
        {-44.f, -140.f, 16.f, 17.f, 15.f, false},
        {-88.f, -140.f, 18.f, 24.f, 16.f, true },
        { 44.f, -140.f, 16.f, 17.f, 15.f, false},
        { 88.f, -140.f, 18.f, 24.f, 16.f, true }
    };



    // =====================================
    // SIDE ROADS
    // =====================================

    SceneGeometry roads;


    for (float side : {-1.f, 1.f}) {

        for (float xOffset : {25.f, 51.f, 77.f}) {
            roads.box(
                { side * xOffset, -0.015f, -48.f },
                { 5.f, 0.055f, 205.f }
            );
        }
    }


    attach(
        root,
        roads,
        { 0.48f, 0.53f, 0.60f },
        { 0.015f, 0.02f, 0.03f },
        asphaltTexture.get()
    );

    // =====================================
    // GENERATE BUILDINGS
    // =====================================

    for (int i = 0; i < 12; ++i) {

        const BuildingInfo& b = buildings[i];

        auto group = std::make_unique<Node>();

        group->position = {
            b.x, 0.f, b.z
        };

        // Each building has its own lighting ID.
        group->wowBuildingId = i;

        Node* building =
            root.addChild(std::move(group));

        SceneGeometry body;
        SceneGeometry roof;
        SceneGeometry windows;
        SceneGeometry darkWindows;
        SceneGeometry details;

        // Main building structure
        body.box(
            { 0.f, 0.f, 0.f },
            { b.width, b.height, b.depth }
        );

        // Decorative roof edge
        roof.box(
            { 0.f, b.height, 0.f },
            {
                b.width + 0.65f,
                0.35f,
                b.depth + 0.65f
            }
        );

        // Snowy roof
        roof.box(
            { 0.f, b.height + 0.35f, 0.f },
            {
                b.width + 0.85f,
                0.15f,
                b.depth + 0.85f
            }
        );

        // Office buildings get a top structure.
        if (b.office) {
            details.box(
                { 0.f, b.height + 0.5f, 0.f },
                { b.width * 0.35f, 1.5f, b.depth * 0.35f }
            );
        }

        // =====================================
        // WINDOWS
        // =====================================

        const float floorHeight = 3.1f;

        for (float y = 2.0f;
            y < b.height - 1.0f;
            y += floorHeight) {

            // Front and back windows
            for (int col = -2; col <= 2; ++col) {

                float x = col * (b.width / 6.0f);

                bool lit =
                    ((i + col + static_cast<int>(y)) % 4 != 0);

                SceneGeometry& target =
                    lit ? windows : darkWindows;

                target.box(
                    { x, y, b.depth * 0.5f + 0.055f },
                    { 1.25f, 1.55f, 0.075f }
                );

                target.box(
                    { x, y, -b.depth * 0.5f - 0.055f },
                    { 1.25f, 1.55f, 0.075f }
                );
            }

            // Side windows
            for (int col = -1; col <= 1; ++col) {

                float z = col * (b.depth / 4.0f);

                bool lit =
                    ((i + col + static_cast<int>(y)) % 3 != 0);

                SceneGeometry& target =
                    lit ? windows : darkWindows;

                target.box(
                    { b.width * 0.5f + 0.055f, y, z },
                    { 0.075f, 1.55f, 1.25f }
                );

                target.box(
                    { -b.width * 0.5f - 0.055f, y, z },
                    { 0.075f, 1.55f, 1.25f }
                );
            }
        }

        // =====================================
        // ATTACH MATERIALS
        // =====================================

        if (b.office) {

            attach(
                *building,
                body,
                { 0.42f, 0.59f, 0.76f },
                { 0.025f, 0.055f, 0.09f },
                blueGlassTexture.get()
            );

        }
        else {

            attach(
                *building,
                body,
                { 0.70f, 0.74f, 0.79f },
                { 0.035f, 0.045f, 0.055f },
                brickTexture.get()
            );
        }

        attach(
            *building,
            roof,
            { 0.85f, 0.89f, 0.95f },
            { 0.08f, 0.10f, 0.13f },
            snowTexture.get()
        );

        attach(
            *building,
            details,
            { 0.36f, 0.43f, 0.51f },
            { 0.025f, 0.035f, 0.055f },
            silverMetalTexture.get()
        );

        // Warm illuminated windows
        Node* litNode = attach(
            *building,
            windows,
            { 1.0f, 0.75f, 0.35f },
            { 0.85f, 0.48f, 0.12f }
        );

        if (litNode) {
            litNode->wowWindow = true;
        }

        // Permanently dark windows
        attach(
            *building,
            darkWindows,
            { 0.10f, 0.15f, 0.23f },
            { 0.005f, 0.012f, 0.025f }
        );
    }
}


void Scene::update(float time) {
    if (beaconPivot) {
        beaconPivot->rotation.y = time * 0.35f;
    }
}

void Scene::draw(const Shader& shader) const {
    root.draw(
        shader,
        glm::mat4(1.f)
    );
}

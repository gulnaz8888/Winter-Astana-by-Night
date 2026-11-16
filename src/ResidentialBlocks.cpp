
#include "Scene.h"
#include <cmath>

void Scene::buildResidentialBlocks() {

    struct BuildingInfo {
        float x, z;
        float width, height, depth;
        bool office;
    };

    // Preserve the positions you approved.
    const BuildingInfo buildings[12] = {
        {-35.f,  10.f, 14.f, 10.f, 13.f, false},
        {-65.f,  10.f, 16.f, 12.f, 14.f, false},
        { 35.f,  10.f, 14.f, 10.f, 13.f, false},
        { 65.f,  10.f, 16.f, 12.f, 14.f, false},

        {-64.f, -32.f, 16.f, 16.f, 15.f, false},
        {-92.f, -32.f, 17.f, 19.f, 15.f, true },
        { 64.f, -32.f, 16.f, 16.f, 15.f, false},
        { 92.f, -32.f, 17.f, 19.f, 15.f, true },

        {-44.f, -140.f, 16.f, 17.f, 15.f, false},
        {-88.f, -140.f, 18.f, 24.f, 16.f, true },
        { 44.f, -140.f, 16.f, 17.f, 15.f, false},
        { 88.f, -140.f, 18.f, 24.f, 16.f, true }
    };

    // Prototype dimensions used by Gulnaz.
    const float w = 20.f;
    const float d = 16.f;
    const float h = 11.2f;

    SceneGeometry masonry, trim, roof, snow;
    SceneGeometry frames, glass, glow, rails;

    // Main structure and decorative projections.
    masonry.box({ 0, 0, 0 }, { w, h, d });
    masonry.box(
        { 0, 0, d / 2 + 0.45f },
        { 5.f, h + 0.65f, 0.9f }
    );

    for (float side : {-1.f, 1.f}) {
        masonry.box(
            { side * (w / 2 + 0.45f), 0, 0 },
            { 0.9f, h + 0.65f, 4.3f }
        );
    }

    // Horizontal architectural trim.
    for (float y : {0.35f, 3.65f, 6.65f, 9.65f, 11.2f}) {
        trim.box({ 0, y, 0 }, { w + 0.45f, 0.23f, d + 0.45f });
        trim.box(
            { 0, y, d / 2 + 0.52f },
            { 5.35f, 0.25f, 1.12f }
        );
    }

    // Sloped roof from Gulnaz's residential design.
    const glm::vec3 bottom[4] = {
        {-w / 2 - 0.4f, h + 0.35f,  d / 2 + 0.4f},
        { w / 2 + 0.4f, h + 0.35f,  d / 2 + 0.4f},
        { w / 2 + 0.4f, h + 0.35f, -d / 2 - 0.4f},
        {-w / 2 - 0.4f, h + 0.35f, -d / 2 - 0.4f}
    };

    const glm::vec3 top[4] = {
        {-w / 2 + 1.6f, h + 2.15f,  d / 2 - 1.5f},
        { w / 2 - 1.6f, h + 2.15f,  d / 2 - 1.5f},
        { w / 2 - 1.6f, h + 2.15f, -d / 2 + 1.5f},
        {-w / 2 + 1.6f, h + 2.15f, -d / 2 + 1.5f}
    };

    for (int i = 0; i < 4; ++i) {
        int j = (i + 1) % 4;
        roof.quad(bottom[i], bottom[j], top[j], top[i]);

        snow.beam(
            bottom[i] + glm::vec3(0, 0.05f, 0),
            bottom[j] + glm::vec3(0, 0.05f, 0),
            0.09f
        );
    }

    snow.quad(top[0], top[1], top[2], top[3]);

    // Roof dormers.
    for (float offset : {-6.6f, 6.6f}) {
        trim.box(
            { offset, h + 0.6f, d / 2 - 0.15f },
            { 2.2f, 1.2f, 0.7f }
        );

        glass.box(
            { offset, h + 0.78f, d / 2 + 0.23f },
            { 1.15f, 0.85f, 0.08f }
        );

        roof.triangle(
            { offset - 1.3f, h + 1.8f, d / 2 + 0.25f },
            { offset + 1.3f, h + 1.8f, d / 2 + 0.25f },
            { offset, h + 2.65f, d / 2 + 0.25f }
        );
    }

    // Detailed facade with windows and balconies.
    auto facade = [&](glm::vec3 origin,
        glm::vec3 u,
        glm::vec3 n,
        int columns,
        float span) {

            auto box = [&](SceneGeometry& g,
                float along, float y, float offset,
                float width, float height, float depth) {

                    g.box(
                        origin + u * along + n * offset
                        + glm::vec3(0, y, 0),
                        glm::abs(u) * width
                        + glm::vec3(0, height, 0)
                        + glm::abs(n) * depth
                    );
                };

            for (int floor = 0; floor < 3; ++floor) {
                float y = 1.f + floor * 3.f;

                for (int col = 0; col < columns; ++col) {
                    float along =
                        -span / 2.f
                        + span * (col + 0.5f) / columns;

                    bool center = std::abs(along) < 2.3f;
                    float front = center ? 1.03f : 0.07f;
                    float ww = center ? 1.65f : 1.35f;

                    box(trim, along, y - 0.12f,
                        front, ww + 0.42f, 2.42f, 0.28f);

                    box(glass, along, y,
                        front + 0.17f, ww, 2.12f, 0.045f);

                    if ((floor + col) % 4 != 0) {
                        box(glow, along, y + 0.12f,
                            front + 0.20f, ww - 0.20f,
                            1.82f, 0.02f);
                    }

                    box(frames, along, y,
                        front + 0.23f, 0.055f, 2.12f, 0.045f);

                    box(frames, along, y + 1.30f,
                        front + 0.24f, ww, 0.055f, 0.045f);

                    box(trim, along, y - 0.22f,
                        front + 0.24f, ww + 0.65f,
                        0.13f, 0.65f);

                    if (floor > 0 && (center || col % 2 == 0)) {
                        box(trim, along, y - 0.25f,
                            front + 0.48f, ww + 0.8f,
                            0.13f, 1.05f);

                        box(rails, along, y + 0.05f,
                            front + 1.01f, ww + 0.60f,
                            0.055f, 0.065f);

                        box(rails, along, y + 0.70f,
                            front + 1.01f, ww + 0.60f,
                            0.055f, 0.065f);

                        for (int k = 0; k < 7; ++k) {
                            box(rails,
                                along - (ww + 0.6f) / 2.f
                                + k * (ww + 0.6f) / 6.f,
                                y + 0.05f,
                                front + 1.01f,
                                0.035f, 0.7f, 0.04f);
                        }
                    }
                }
            }

            for (float sign : {-1.f, 1.f}) {
                box(trim, sign * (span / 2 - 0.25f),
                    0.4f, 0.15f, 0.48f, 10.75f, 0.30f);
            }

            box(trim, 0, h + 0.18f,
                0.95f, 5.3f, 0.38f, 1.2f);
        };

    // Front and both side facades.
    facade({ 0, 0, d / 2 }, { 1, 0, 0 },
        { 0, 0, 1 }, 6, w - 1.f);

    facade({ -w / 2, 0, 0 }, { 0, 0, 1 },
        { -1, 0, 0 }, 5, d - 1.f);

    facade({ w / 2, 0, 0 }, { 0, 0, 1 },
        { 1, 0, 0 }, 5, d - 1.f);

    // Upload prototype geometry once.
    auto createMesh = [&](const SceneGeometry& geometry)
        -> const Mesh* {

        meshes.push_back(
            std::make_unique<Mesh>(
                geometry.vertices,
                geometry.indices
            )
        );

        return meshes.back().get();
        };

    const Mesh* bodyMesh = createMesh(masonry);
    const Mesh* trimMesh = createMesh(trim);
    const Mesh* roofMesh = createMesh(roof);
    const Mesh* snowMesh = createMesh(snow);
    const Mesh* frameMesh = createMesh(frames);
    const Mesh* glassMesh = createMesh(glass);
    const Mesh* glowMesh = createMesh(glow);
    const Mesh* railsMesh = createMesh(rails);

    // Reuse the meshes at all 12 original positions.
    for (int i = 0; i < 12; ++i) {
        const BuildingInfo& b = buildings[i];

        auto group = std::make_unique<Node>();
        group->position = { b.x, 0.f, b.z };
        group->wowBuildingId = i;

        Node* building = root.addChild(std::move(group));

        glm::vec3 modelScale(
            b.width / w,
            b.height / h,
            b.depth / d
        );

        auto addPart = [&](const Mesh* mesh,
            glm::vec3 color,
            glm::vec3 emission,
            const Texture* texture,
            bool animatedWindow = false) {

                auto part = std::make_unique<Node>();

                part->mesh = mesh;
                part->scale = modelScale;
                part->color = color;
                part->emissive = emission;
                part->texture = texture;
                part->wowWindow = animatedWindow;

                building->addChild(std::move(part));
            };

        addPart(
            bodyMesh,
            b.office ? glm::vec3(0.42f, 0.59f, 0.76f)
            : glm::vec3(0.75f, 0.80f, 0.87f),
            { 0.07f, 0.09f, 0.12f },
            b.office ? blueGlassTexture.get()
            : brickTexture.get()
        );

        addPart(trimMesh, { 0.83f, 0.80f, 0.73f },
            { 0.13f, 0.13f, 0.13f }, nullptr);

        addPart(roofMesh, { 0.10f, 0.15f, 0.22f },
            { 0.02f, 0.03f, 0.05f }, nullptr);

        addPart(snowMesh, { 0.78f, 0.85f, 0.96f },
            { 0.16f, 0.19f, 0.25f }, nullptr);

        addPart(glassMesh, { 0.08f, 0.16f, 0.24f },
            { 0.015f, 0.035f, 0.06f },
            blueGlassTexture.get());

        addPart(glowMesh, { 0.90f, 0.78f, 0.53f },
            { 0.35f, 0.22f, 0.08f },
            nullptr, true);

        addPart(frameMesh, { 0.74f, 0.77f, 0.79f },
            { 0.11f, 0.12f, 0.14f }, nullptr);

        addPart(railsMesh, { 0.09f, 0.11f, 0.14f },
            { 0.015f, 0.02f, 0.03f }, nullptr);
    }
}

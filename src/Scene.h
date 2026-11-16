
#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Node.h"
#include "Shader.h"
#include "Texture.h"
#include "SceneGeometry.h"

class Scene {
public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void update(float time);
    void draw(const Shader& shader) const;

private:
    Mesh sphere;
    std::vector<std::unique_ptr<Mesh>> meshes;

    std::unique_ptr<Texture> snowTexture;
    std::unique_ptr<Texture> asphaltTexture;
    std::unique_ptr<Texture> brickTexture;
    std::unique_ptr<Texture> goldGlassTexture;
    std::unique_ptr<Texture> silverMetalTexture;
    std::unique_ptr<Texture> blueGlassTexture;
    std::unique_ptr<Texture> lampGlassTexture;
    std::unique_ptr<Texture> pineTexture;

    Node root;
    Node* beaconPivot = nullptr;

    Node* attach(
        Node& parent,
        const SceneGeometry& geometry,
        glm::vec3 color,
        glm::vec3 emission = glm::vec3(0.f),
        const Texture* texture = nullptr
    );

    void buildGround();
    void buildAvenue();
    void buildLamps();
    void buildSkyline();
    void buildExtraCity();
    void buildMosque();
    void buildKhanShatyr();
    void buildBaiterek();
};

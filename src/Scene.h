#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Node.h"
#include "Shader.h"

class Scene {
public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void update(float time);
    void draw(const Shader& shader) const;

private:
    Mesh plane;
    Mesh cube;
    Mesh cylinder;
    Mesh taper;
    Mesh sphere;
    Node root;
    Node* beaconPivot = nullptr;

    Node* add(Node& parent, const Mesh& mesh, glm::vec3 position, glm::vec3 scale,
              glm::vec3 color, glm::vec3 emissive = glm::vec3(0.f));
    void buildGround();
    void buildBuildings();
    void buildLamps();
    void buildBaiterek();
    void buildMoon();
};

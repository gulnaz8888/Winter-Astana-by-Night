#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"
#include "Node.h"
#include "Shader.h"

class ExpoScene {
public:
    ExpoScene();
    ExpoScene(const ExpoScene&) = delete;
    ExpoScene& operator=(const ExpoScene&) = delete;

    void update(float time);
    void draw(const Shader& shader) const;

private:
    struct Twinkle {
        Node* node;
        glm::vec3 base;
        float phase;
    };

    Mesh plane;
    Mesh cube;
    Mesh cylinder;
    Mesh taper;
    Mesh cone;
    Mesh sphere;
    Mesh bigSphere;
    Mesh torus;
    Node root;
    Node* starPivot = nullptr;
    std::vector<Node*> spinners;
    std::vector<Twinkle> twinkles;

    Node* add(Node& parent, const Mesh& mesh, glm::vec3 position, glm::vec3 scale,
              glm::vec3 color, glm::vec3 emissive = glm::vec3(0.f));
    Node* addGroup(Node& parent, glm::vec3 position);
    Node* addRod(Node& parent, glm::vec3 from, glm::vec3 to, float thickness, glm::vec3 color);
    void buildGround();
    void buildPodium();
    void buildNurAlem();
    void buildTree();
    void buildOrnament(glm::vec3 position, float diameter, glm::vec3 ring, glm::vec3 core);
    void buildArches(float x0, float x1, float z);
    void buildBench(glm::vec3 position);
    void buildPerson(glm::vec3 position, float height, glm::vec3 coat, glm::vec3 hat, float facing);
    void buildCrowd();
};

#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
#include "Shader.h"

struct Node {
    const Mesh* mesh = nullptr;
    glm::vec3 position{0.f};
    glm::vec3 rotation{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 color{1.f};
    glm::vec3 emissive{0.f};
    std::vector<std::unique_ptr<Node>> children;

    Node* addChild(std::unique_ptr<Node> child) {
        children.push_back(std::move(child));
        return children.back().get();
    }

    glm::mat4 localMatrix() const {
        glm::mat4 m(1.f);
        m = glm::translate(m, position);
        m = glm::rotate(m, rotation.y, glm::vec3(0.f, 1.f, 0.f));
        m = glm::rotate(m, rotation.x, glm::vec3(1.f, 0.f, 0.f));
        m = glm::rotate(m, rotation.z, glm::vec3(0.f, 0.f, 1.f));
        return m;
    }

    void draw(const Shader& shader, const glm::mat4& parentWorld) const {
        glm::mat4 world = parentWorld * localMatrix();
        if (mesh) {
            shader.setMat4("uModel", glm::scale(world, scale));
            shader.setVec3("uColor", color);
            shader.setVec3("uEmissive", emissive);
            mesh->draw();
        }
        for (const auto& child : children) {
            child->draw(shader, world);
        }
    }
};

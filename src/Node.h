
#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

struct Node {
    const Mesh* mesh = nullptr;
    const Texture* texture = nullptr;

    glm::vec3 position{0.f};
    glm::vec3 rotation{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 color{1.f};
    glm::vec3 emissive{0.f};
    glm::vec3 uvScale{1.f};

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
            shader.setVec3("uUVScale", uvScale);

            shader.setInt("uUseTexture", texture != nullptr ? 1 : 0);
            shader.setInt("uTexture", 0);

            if (texture) {
                texture->bind(0);
            } else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            mesh->draw();
        }

        for (const auto& child : children) {
            child->draw(shader, world);
        }
    }
};

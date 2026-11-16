#pragma once

#include <memory>
#include <vector>
#include "Mesh.h"
#include "Node.h"
#include "Shader.h"

class RiverfrontScene {
public:
    RiverfrontScene();
    RiverfrontScene(const RiverfrontScene&) = delete;
    RiverfrontScene& operator=(const RiverfrontScene&) = delete;
    void draw(const Shader& shader) const;

private:

    std::unique_ptr<Texture> towerPhoto;
    std::unique_ptr<Texture> iceTexture;
    std::vector<std::unique_ptr<Mesh>> meshes;
    Node root;
};

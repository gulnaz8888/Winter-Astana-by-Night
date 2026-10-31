
#pragma once

#include "Shader.h"
#include <glm/glm.hpp>

class Lights {
public:
    // Send lighting parameters to the shader
    static void apply(
        const Shader& shader,
        const glm::vec3& cameraPosition,
        float time
    );
};

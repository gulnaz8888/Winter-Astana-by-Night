
#pragma once

#include "Shader.h"
#include <glm/glm.hpp>

class Lights {
public:
    static void apply(
        const Shader& shader,
        const glm::vec3& cameraPosition,
        float time,
        int sceneIndex,
        bool enabled
    );
};

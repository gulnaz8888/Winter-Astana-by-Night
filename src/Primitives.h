#pragma once
#include "Mesh.h"

namespace Primitives {
Mesh plane();
Mesh cube();
Mesh cylinder(int segments = 32, float topRadius = 0.5f);
Mesh sphere(int stacks = 24, int slices = 32);
Mesh torus(int segments = 48, int sides = 8, float tubeRadius = 0.016f);
}

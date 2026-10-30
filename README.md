# Winter Astana by Night
Real-time 3D scene in OpenGL 3.3 core: snowfall particles + emissive lights.
Computer Graphics Fundamentals, Final Project, AITU.

## Build
Requirements: CMake 3.16+, C++17 compiler, OpenGL 3.3+.
All libraries (GLFW, GLM, GLAD, stb_image) are included in `external/`.

    cmake -S . -B build
    cmake --build build -j
    ./build/winter

## Controls
TBD

## Features (requirements R1-R8)
| Req | Feature | Where |
|-----|---------|-------|
| R1 | Core profile, VAO/VBO/EBO, own shaders | src/Mesh.*, shaders/ |
| R2 | Scene with 5+ objects | src/Scene.* |
| R3 | MVP matrices, parent-child hierarchy | src/Camera.*, src/Node.h |
| R4 | Blinn-Phong, 2+ lights | TBD |
| R5 | Texture / skybox / procedural effect | TBD |
| R6 | Camera + 2 scene interactions | TBD |
| R7 | FPS counter in window title | TBD |
| R8 | Build steps above | README |

## Team
| Member | Role |
|--------|------|
| Gulnaz Yeskermes | Scene architecture, primitives, MVP, hierarchy |
| Makhanbetiyar Begina | Shaders, lighting, and emissive materials (R1, R4, R5)  |
| Amralina Kamila | Camera, interactivity, and texturing (R5, R6)  |
| Anetbayeva Dilyara | Particle system, performance, and repository (R7, R8, Stretch Goals)  |



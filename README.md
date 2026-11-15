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
| Key | Action |
|-----|--------|
| `1` | Winter promenade and Baiterek |
| `2` | EXPO and Nur Alem |
| `3` | Ishim riverfront |
| `L` | Toggle the two point lights |
| `Esc` | Exit |

The Baiterek and riverfront scenes currently use fixed camera presets. The EXPO
scene retains its automatic camera movement. Interactive camera controls are a
separate team task.

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



## Scene geometry

The first scene contains a snowy pedestrian promenade with evergreens, benches,
street lamps and hanging lights. Baiterek uses a central shaft, 24 flared ribs,
diagonal lattice and a gold sphere with a pane grid. Two gold towers, Khan Shatyr
and a mosque form the background; two nearby buildings frame the sides.

The third scene contains a frozen river, snowy banks, a staircase and decorative
railing, exactly three curved Northern Lights towers, residential frontage,
holiday stalls, visitors, skaters and hanging lights along both banks. The user-
supplied photograph is mapped to facade regions of the three towers. Static
repeated details are batched by material.

Scene architecture and geometry are in `Scene.cpp`, `SceneGeometry.h` and
`RiverfrontScene.cpp`. Shared shaders, lighting and snowfall retain their existing
implementation. Ice texturing, interactive cameras and fireworks remain team
handoff tasks.

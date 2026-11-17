# Winter Astana by Night

**Computer Graphics Fundamentals - Final Project**  
**Astana IT University**  
**Repository:** https://github.com/gulnaz8888/Winter-Astana-by-Night

Winter Astana by Night is a real-time interactive 3D application developed in **C++17**, **OpenGL 3.3 Core Profile**, and **GLSL 330**. It presents a stylized winter evening in Astana through three destinations selected from an animated LRT carriage: the Baiterek promenade, EXPO / Nur Alem, and the Ishim riverfront. The project demonstrates modern graphics-pipeline concepts, hierarchical transformations, per-fragment lighting, texture mapping, animation, and particle effects.

## Features and scenes

**1 — Baiterek / Winter City.** A snowy promenade with the Baiterek monument, Khan Shatyr, a mosque, architectural towers, detailed residential buildings, lamps, benches, and trees. Press `P` to start an approximately eight-second cinematic: the camera rises over the city while building lights illuminate progressively.

**2 — EXPO / Nur Alem.** A stylized spherical EXPO landmark with architectural panels, a glazed podium, warm entrance lighting, festive details, and gentle automatic camera motion until manual camera control is used.

**3 — Ishim Riverfront.** A frozen river with textured ice, three curved Northern Lights towers, surrounding buildings, winter decorations, snowfall, and animated fireworks.

**LRT destination menu.** An animated 3D carriage with selectable destinations and mouse-based selection. A welcome voice announcement plays on application startup, and a destination-specific station announcement plays when a scene is selected. Background music (credited under CC BY 4.0) starts on entering a scene, continues across scene changes, can be toggled with `V`, and stops when returning to the menu. The music volume is reduced during voice announcements and restored afterward.

## Build instructions

### Requirements
- CMake **3.16 or later**.
- A **C++17 compiler** (e.g., MSVC / Visual Studio on Windows).
- A graphics driver supporting **OpenGL 3.3 Core Profile or later**.
- An audio output device for background music (the renderer can still be inspected independently of music).

Project dependencies are supplied under `external/`: **GLFW**, **GLM**, **GLAD**, **stb_image**, and **miniaudio**. The project does not use Python or a game engine.

### Windows - Visual Studio / Developer PowerShell
Run these commands from the repository root:

```powershell
cmake -S . -B build
cmake --build build --config Debug -j 4
.\build\Debug\winter.exe
```

For a Release build:

```powershell
cmake -S . -B build
cmake --build build --config Release -j 4
.\build\Release\winter.exe
```

### macOS / Linux (with compatible compiler and OpenGL support)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/winter
```

**Important:** Run the commands from the repository root as shown: the application loads `assets/` and `shaders/` by relative path. CMake also copies these directories beside the executable after building. If audio is unavailable, check the console for the music-player diagnostic.

## Controls

| Key / action | Function |
|---|---|
| `1` | Open Baiterek / Winter City |
| `2` | Open EXPO / Nur Alem |
| `3` | Open Ishim Riverfront |
| **Left mouse drag** | Orbit the camera in a scene |
| **Mouse wheel / touchpad scroll** | Zoom the orbit camera |
| `A` / `D` | Rotate camera horizontally |
| `W` / `S` | Rotate camera vertically |
| `Home` | Reset the camera to the scene's default view |
| `L` | Toggle point lighting |
| `N` | Toggle automatic snow coverage on upward-facing surfaces |
| `←` / `→` | Set wind direction left / right |
| `↑` / `↓` | Increase / decrease wind strength |
| `R` | Reset wind settings |
| `F` | Toggle fireworks (visible in Riverfront) |
| `P` | Start / restart the cinematic Wow Moment (Winter City) |
| `M` | Return to the LRT menu; background music stops |
| `V` | Toggle background music on/off; playback restarts when turned on again |
| `Esc` | Exit the application |
| **Left click in LRT menu** | Select a destination |

The camera switches from preset/automatic movement to manual orbit control when the user interacts with it. During the Baiterek cinematic, manual camera input cancels the animation. The `V` key toggles music on/off; when it is re-enabled, playback restarts from the beginning rather than resuming from the previous position.

## Technical implementation and project requirements

| Requirement | Implementation | Main source files |
|---|---|---|
| **R1 - Modern pipeline** | OpenGL 3.3 Core Profile, custom GLSL vertex/fragment shaders, VAO/VBO/EBO, indexed meshes | `src/Mesh.*`, `src/Shader.*`, `shaders/` |
| **R2 - Scene setup** | Multiple distinct procedural 3D objects and landmarks in each destination | `src/Scene.cpp`, `src/ExpoScene.cpp`, `src/RiverfrontScene.cpp` |
| **R3 - Transformations** | Model, view and projection matrices; parent–child scene hierarchy and animated pivots | `src/Camera.*`, `src/Node.h`, `shaders/basic.vert` |
| **R4 - Lighting/shading** | Per-fragment Blinn–Phong ambient, diffuse and specular lighting; at least two point lights, including moving/toggleable lighting; emissive materials | `src/Lights.*`, `shaders/basic.frag` |
| **R5 - Texturing** | UV-mapped surface materials, repeated textures, mipmaps and procedural effects | `src/Texture.*`, `src/SceneGeometry.h`, `shaders/basic.frag` |
| **R6 - Interaction** | Keyboard and mouse orbit camera, destination switching, light/snow/wind/fireworks controls, cinematic sequence | `src/main.cpp`, `src/LrtMenu.*` |
| **R7 - Performance** | Frame-based animation and FPS counter updated in the window title | `src/main.cpp`, `src/Snow.*`, `src/Fireworks.*` |
| **R8 - Reproducible build** | CMake-based C++ project with dependencies, shaders and assets included in the repository | `CMakeLists.txt`, `external/`, this README |

**LRT audio:** `welcome.mp3` plays on launch; `Baiterek_station.mp3`, `Expo_station.mp3`, and `Ishim_station.mp3` are triggered by the corresponding destination selection. Voice announcements temporarily lower the volume of background music.

**Topic-specific stretch effects:** normal-based snow coverage on upward-facing surfaces; adjustable snowfall wind; animated riverfront fireworks. **Wow Moment:** a rising camera and block-by-block lighting reveal in Winter City.

### Performance verification

The project opens a **1280 × 720** window and displays a live FPS counter in the window title. The team's preliminary report records local rendering tests, but their measurements should not be treated as verification on the teaching assistant's computer. Before final submission, test the normal application visibly on the target machine and confirm **at least 30 FPS** in each scene; record the test environment and results in the report.

## Team and contributions

| Team member | Group | Contributions |
|---|---|---|
| **Gulnaz Yeskermes** | SE-2425 | Scene architecture, procedural primitives and landmark geometry, MVP transformations, scene hierarchy, residential facade design, animated LRT menu and EXPO architectural refinements |
| **Makhanbetiyar Begina** | SE-2427 | GLSL lighting and shading, two-point-light system, emissive and architectural lighting, eight-second city cinematic, sequential light reveal, residential-building placement and integration |
| **Amralina Kamila** | SE-2427 | Texture mapping and materials, visual scene improvements, keyboard-and-mouse orbit camera controls and interaction integration |
| **Anetbayeva Dilyara** | SE-2427 | Snow particle system, wind controls, automatic surface snow, fireworks, river ice texture, performance-related work and Christmas background-music integration |

Development was organized through Git branches and reviewed pull requests. All team members should be able to explain the rendering and interaction code during the defense.

## Source code structure

- `src/main.cpp` - initialization, render loop, scene switching, keyboard/mouse input, FPS, camera and music integration.
- `src/Scene.cpp`, `src/ResidentialBlocks.cpp` - Baiterek scene, detailed residential geometry and Wow Moment building identifiers.
- `src/ExpoScene.cpp`, `src/RiverfrontScene.cpp` - destination-specific geometry and animation.
- `src/Mesh.*`, `src/SceneGeometry.h`, `src/Node.h` - procedural geometry, GPU buffers and hierarchy.
- `src/Lights.*`, `shaders/basic.vert`, `shaders/basic.frag` - transforms, per-fragment lighting, emissive materials and automatic snow coverage.
- `src/Snow.*`, `src/Fireworks.*` - particle effects.
- `src/LrtMenu.*`, `shaders/menu_*` - animated LRT and interactive selection.
- `src/MusicPlayer.*`, `assets/audio/` - looping Christmas music and welcome/station voice announcements through miniaudio.
- `assets/textures/`, `assets/menu/` - textures and menu image assets.

## Third-party assets and acknowledgments

See [CREDITS.md](CREDITS.md) for libraries, music, assets and attribution information. The Christmas track is **“We Wish You a Merry Christmas” by Kevin MacLeod**, provided under **Creative Commons Attribution 4.0**; the track and license links are documented in `CREDITS.md`.

**Voice announcement attribution:** The repository also includes `assets/audio/welcome.mp3`, `Baiterek_station.mp3`, `Expo_station.mp3`, and `Ishim_station.mp3`. Their recording/generation method, authorship, and licensing are awaiting team confirmation and are explicitly flagged in [CREDITS.md](CREDITS.md).

**Asset attribution status:** team-generated AI textures and LRT menu PNGs are documented in [CREDITS.md](CREDITS.md). The internet-sourced photograph `assets/textures/northern_lights_reference.png` is used as a Riverfront facade texture. A closely matching photograph was located on Krisha.kz, but the original photographer, copyright holder, and permission to distribute/use the image have **not** been verified. The team must obtain authorization or replace the asset with an original or appropriately licensed texture before the final public release. Merely citing a source is not permission to redistribute an image.

## AI Use Note

The team used **OpenAI ChatGPT and Codex** as supporting tools to explore suitable graphics libraries and rendering techniques, understand computer-graphics concepts and mathematics, debug and refine parts of the C++ and GLSL implementation, and improve the structure and wording of the written report. According to the team, **Amralina Kamila generated and integrated architectural and environmental material textures using ChatGPT**, and the six LRT menu PNG graphics in `assets/menu/` were also AI-generated. The exception is `assets/textures/northern_lights_reference.png`, which is an internet-sourced photograph rather than an AI-generated texture. See [CREDITS.md](CREDITS.md) for the itemized asset list and attribution status.

Team members provided visual references, selected scene designs and parameters, integrated code through Git, adjusted the appearance of the application, and reviewed output and behavior during local tests. AI-generated suggestions were examined and revised rather than treated as automatically verified results. The application itself is implemented in **C++ and GLSL**; no Python code is required to build or run it.

## Current scope and limitations

The application uses stylized geometry rather than geographically exact reconstruction. It does not implement physically based reflection/refraction, full dynamic shadows, collision detection, or physically simulated snow and ice. Local performance does not guarantee the same FPS on different hardware. This documentation reflects the source reviewed on **17 November 2026**. Feature presence in source code is not a substitute for final runtime testing; recheck controls and performance after any changes and before tagging the release.

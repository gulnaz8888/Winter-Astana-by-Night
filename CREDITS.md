# Credits and Asset Attribution

**Project:** Winter Astana by Night  
**Course:** Computer Graphics Fundamentals — Astana IT University  
**Repository:** https://github.com/gulnaz8888/Winter-Astana-by-Night

This document records the external software, audio, image assets, and architectural references associated with the project. It distinguishes **assets distributed with the application** from **photographs used only as visual references**. Credit to a source does not by itself grant permission to redistribute a copyrighted work.

## 1. Third-Party Software Libraries

| Library | Creator / maintainers | Source | License |
|---|---|---|---|
| GLFW 3.4 | GLFW contributors | https://www.glfw.org/ | zlib/libpng |
| GLM 1.0.1 | G-Truc Creation | https://github.com/g-truc/glm | MIT |
| GLAD | David Herberth / GLAD contributors | https://github.com/Dav1dde/glad | MIT |
| stb_image | Sean Barrett and contributors | https://github.com/nothings/stb | Public domain / MIT dual option |
| miniaudio | David Reid | https://github.com/mackron/miniaudio | Public domain / MIT No Attribution (MIT-0) dual option |

These libraries are included in the repository under `external/` and are used for window creation and input handling, mathematics, OpenGL function loading, image loading, and audio playback. Their original copyright and license notices should be retained where supplied.

## 2. Background Music

| File in project | Work | Artist | Source | License |
|---|---|---|---|---|
| `assets/audio/christmas.mp3` | *We Wish You a Merry Christmas* | Kevin MacLeod | https://www.incompetech.com/music/royalty-free/index.html?isrc=USUAN1100369 | Creative Commons Attribution 4.0 International (CC BY 4.0) |

**Required attribution:**  
“We Wish You a Merry Christmas” by Kevin MacLeod (incompetech.com).  
Licensed under Creative Commons Attribution 4.0 International: https://creativecommons.org/licenses/by/4.0/

The music is played in the winter destinations using miniaudio. The `V` key switches music on or off; turning it on again restarts playback. The `M` key returns to the LRT menu, where the music stops.

## 3. AI-Generated LRT Welcome and Station Voice Announcements — Fish Audio

The team confirmed that the following four MP3 files were generated using **Fish Audio** (https://fish.audio/), an AI voice-generation service, by selecting a **pre-existing voice preset labeled “Siri” in the service’s model catalog**. According to the team, they did **not** upload voice samples or create a custom voice clone. The files are distributed in `assets/audio/` and played through the project's miniaudio-based audio system. The preset label does not establish that it is an official Apple Siri voice or that Apple endorses this project.

| File | Function | Generation source |
|---|---|---|
| `assets/audio/welcome.mp3` | Welcome announcement at application startup | Fish Audio (AI-generated voice) |
| `assets/audio/Baiterek_station.mp3` | Announces Baiterek selection | Fish Audio (AI-generated voice) |
| `assets/audio/Expo_station.mp3` | Announces EXPO selection | Fish Audio (AI-generated voice) |
| `assets/audio/Ishim_station.mp3` | Announces Ishim Riverfront selection | Fish Audio (AI-generated voice) |

The program reduces background-music volume while an announcement is playing and restores it afterward.

**Usage-rights review:** The source and generation method are team-confirmed: a pre-existing Fish Audio catalog preset labeled “Siri,” rather than a team-created clone. However, the specific preset’s rights, any restrictions associated with it, the account plan, and permission to distribute the generated audio in a public repository have **not** been independently verified. See Fish Audio’s [Terms of Service](https://fish.audio/ru/terms/). The model name alone does not establish permission or affiliation with Apple. Attribution identifies the service, not a verified license.

## 4. AI-Generated Texture Assets

According to the team, **Amralina Kamila** generated and integrated the architectural and environmental material textures with **OpenAI ChatGPT**. These images were used as surface textures and configured through UV mapping and materials in the OpenGL scenes.

The AI-generated texture files reported by the team are:

- `assets/textures/asphalt.png`
- `assets/textures/blue_glass.png`
- `assets/textures/brick.png`
- `assets/textures/glass.png`
- `assets/textures/gold_glass.png`
- `assets/textures/ice.png`
- `assets/textures/lamp_glass.png`
- `assets/textures/pine_needles.png`
- `assets/textures/silver_metal.png`
- `assets/textures/snow.png`

**Exception:** `assets/textures/northern_lights_reference.png` is a photograph obtained from the internet, **not** an AI-generated texture. Its attribution and unresolved reuse permission are documented separately below.

## 5. AI-Generated LRT Menu Graphics

The team confirmed that the PNG graphics used for the LRT destination menu were AI-generated. The files are:

- `assets/menu/baiterek.png`
- `assets/menu/expo.png`
- `assets/menu/hint.png`
- `assets/menu/riverfront.png`
- `assets/menu/subtitle.png`
- `assets/menu/title.png`

The team incorporated these graphics into the menu interface and integrated them with the original 3D LRT scene and its selection controls. OpenAI ChatGPT was used in the team's asset-generation workflow.

## 6. Third-Party Photograph Used as a Texture — Permission Pending

| Field | Details |
|---|---|
| Local file | `assets/textures/northern_lights_reference.png` |
| What it depicts | The three towers of the Grand Alatau residential complex, Astana |
| Current use | Photo-derived facade texture on the three riverfront towers, via UV mapping in `src/RiverfrontScene.cpp` |
| Matching published photograph / source | Krisha.kz — https://krisha.kz/complex/show/astana/grand-alatau/ |
| Related image URL | https://krisha-photos.kcdn.online/content/77/4001560a7011e1345__8484-1-astanagrand-alatau-750x470.jpg |
| Original photographer / rights holder | Not confirmed |
| License or authorization to redistribute/use as a texture | **Not confirmed** |

The team found a closely matching photograph published on Krisha.kz. The exact origin of the copy originally downloaded for this project has not been independently verified. **Krisha.kz is credited as a located publication source, not necessarily as the photographer or copyright holder.**

**Important:** A public image URL and a citation do not establish permission to copy, modify, or redistribute the photograph inside a public GitHub repository. Before the final release, the team should either **obtain authorization from the rights holder** or **replace this photograph with an original or appropriately licensed texture**, update the corresponding project file and attribution, and test the Riverfront scene. This disclosure is not a substitute for permission.

> Note on names: the project source file uses the name `northern_lights_reference.png`, but the matching Krisha.kz listing identifies the photographed complex as **Grand Alatau**. The internal filename does not establish the real-world name of the buildings.

## 7. External Architectural Visual References

The following public images and pages were supplied by team members as potential architectural references for landmark appearance, building proportions, facade details, winter decoration, or scene composition. **Only describe a link as actually used during development if the team confirms that use.** These links are *references*, not permission to reuse the photographs as application assets.

| Website / source | Link |
|---|---|
| iStock — Bayterek Tower at night | https://media.istockphoto.com/id/1736795818/photo/bayterek-tower-at-night-astana-the-capital-of-kazakhstan.jpg |
| Astana TV — city / holiday reference | https://astanatv.kz/cache/imagine/1200/uploads/article/2025/12/29/69528168a5390567165401.jpeg |
| Krisha.kz — Grand Alatau architecture | https://krisha.kz/complex/show/astana/grand-alatau/ |
| Pinterest-hosted architectural reference | https://i.pinimg.com/originals/75/76/41/757641a463f6a25d92dea80a3ab5ddc0.jpg |
| Otzovik-hosted reference | https://i2011.otzovik.com/2011/05/29/82992/img/31809674.jpg |
| NUR.KZ-hosted reference | https://cdn.nur.kz/images/1200x675/fa34edb355d1490e.jpeg?version=1 |
| Kazpravda-hosted reference | https://kazpravda.kz/media/news/2022/12/30/photo_2022-12-30_11-58-31.jpg |
| Yandex-hosted reference 1 | https://avatars.mds.yandex.net/i?id=9c33db14f2995f34190f6cff8fcda1a4_l-16514006-images-thumbs&n=13 |
| Yandex-hosted reference 2 | https://avatars.mds.yandex.net/get-altay/10700956/2a0000018c9bfc3aae8748a0c447827e317d/XXXL |

The publishers or hosting platforms listed above are **not necessarily the original photographers or rights holders**. Unlicensed reference photographs should not be incorporated directly into the distributed project as textures or other image assets without appropriate authorization.

## 8. Original Team Work

The project's C++ and GLSL code, procedurally constructed scene geometry, interactive scene integration, and visual effects were developed and assembled by the team. Contributions are summarized below:

| Team member | Main contributions |
|---|---|
| Gulnaz Yeskermes | Scene architecture, procedural primitives, landmark geometry, MVP transforms, scene hierarchy, and LRT menu |
| Makhanbetiyar Begina | Blinn–Phong lighting and GLSL shaders, emissive materials, residential buildings, and the Baiterek cinematic |
| Amralina Kamila | EXPO materials and texture integration, orbit camera and scene framing, and AI-generated material textures |
| Anetbayeva Dilyara | Snowfall particles, wind, automatic snow coverage, fireworks, river ice, performance work, and music integration |

The project uses ChatGPT and Codex as assistance tools for research, explanation, debugging, code refinement, writing, and image generation as disclosed in the README and final report. The team is responsible for reviewing, integrating, testing, and explaining the delivered work.

## 9. Documentation Figures and University Mark

Figures and scene screenshots used in the report show renders of the team's application. The architecture diagram was prepared for the report. The Astana IT University logo was taken from the university-provided report template and is used for course identification.

---

**Release check:** Verify the applicable usage and redistribution rights of the Fish Audio catalog voice preset used for the four LRT MP3 files; complete the permission review or replacement of `northern_lights_reference.png`; confirm the visual reference list reflects what the team actually used; and keep this file consistent with the final repository assets before tagging `v1.0`.

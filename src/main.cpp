
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <string>
#include <glm/geometric.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "ExpoScene.h"
#include "RiverfrontScene.h"
#include "Sky.h"
#include "Lights.h"
#include "Snow.h"
#include "Fireworks.h"
#include "LrtMenu.h"
#include "MusicPlayer.h"

static void onResize(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

struct OrbitControl {
    glm::vec3 target = {0.f, 0.f, 0.f};
    float distance = 100.f;
    float yaw = 0.f;
    float pitch = 0.f;
    bool manual = false;
    bool dragging = false;
    double lastX = 0.0;
    double lastY = 0.0;
    double scroll = 0.0;

    void reset(const glm::vec3& position, const glm::vec3& lookAt) {
        target = lookAt;
        glm::vec3 delta = position - lookAt;
        distance = glm::length(delta);
        yaw = std::atan2(delta.x, delta.z);
        pitch = std::asin(std::clamp(delta.y / std::max(distance, 0.001f), -1.f, 1.f));
        manual = false;
        dragging = false;
        scroll = 0.0;
    }

    void apply(Camera& camera) const {
        float horizontal = distance * std::cos(pitch);
        camera.target = target;
        camera.position = target + glm::vec3(
            horizontal * std::sin(yaw),
            distance * std::sin(pitch),
            horizontal * std::cos(yaw)
        );
    }
};

static void onScroll(GLFWwindow* window, double, double yOffset) {
    auto* orbit = static_cast<OrbitControl*>(glfwGetWindowUserPointer(window));
    if (orbit) orbit->scroll += yOffset;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "Winter Astana by Night",
        nullptr, nullptr
    );

    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress
    )) {
        std::cerr << "GLAD init failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSwapInterval(1);

    std::cout << "OpenGL: "
              << glGetString(GL_VERSION) << "\n";

    try {
        Shader shader(
            "shaders/basic.vert",
            "shaders/basic.frag"
        );

        Shader skyShader(
            "shaders/sky.vert",
            "shaders/sky.frag"
        );

        Shader snowShader(
            "shaders/snow.vert",
            "shaders/snow.frag"
        );

        Shader fireworksShader(
            "shaders/fireworks.vert",
            "shaders/fireworks.frag"
        );

        Sky sky;
        Scene scene;
        ExpoScene expoScene;
        RiverfrontScene riverfrontScene;

        LrtMenu menu;
        ChristmasMusicPlayer music("assets/audio/christmas.mp3");
        bool menuOpen = true;
        bool mouseWasPressed = false;

        Camera camera;
        camera.position = {0.f, 6.5f, 43.f};
        camera.target = {0.f, 22.f, -60.f};

        OrbitControl orbit;
        glfwSetWindowUserPointer(window, &orbit);
        glfwSetScrollCallback(window, onScroll);

        Snow snow(3500);
        Fireworks fireworks;

        int activeScene = 0;

        bool lightingEnabled = true;
        bool lWasPressed = false;

        // Cinematic Wow Moment
        bool wowActive = false;
        bool pWasPressed = false;
        float wowStartTime = 0.0f;
        float wowProgress = 1.0f;
        const float wowDuration = 8.0f;

        bool fireworksEnabled = true;
        bool fWasPressed = false;

        bool autoSnowEnabled = true;
        bool nWasPressed = false;

        float windDirection = 1.f;
        float windStrength = 0.5f;

        bool leftWasPressed = false;
        bool rightWasPressed = false;
        bool upWasPressed = false;
        bool downWasPressed = false;
        bool rWasPressed = false;

        snow.setWind(
            windDirection * windStrength, 0.f
        );

        const int snowCounts[3] = {
            3500, 3500, 1000
        };

        const glm::vec3 fogColors[3] = {
            {0.055f, 0.085f, 0.15f},
            {0.30f, 0.22f, 0.17f},
            {0.035f, 0.065f, 0.12f}
        };

        const float fogDensities[3] = {
            0.0023f, 0.0025f, 0.0018f
        };

        const glm::vec3 skyHorizon[3] = {
            {0.055f, 0.085f, 0.15f},
            {0.33f, 0.24f, 0.18f},
            {0.035f, 0.065f, 0.12f}
        };

        const glm::vec3 skyTop[3] = {
            {0.005f, 0.01f, 0.028f},
            {0.20f, 0.14f, 0.11f},
            {0.004f, 0.009f, 0.025f}
        };

        const float skyStars[3] = {
            0.32f, 0.f, 0.55f
        };

        const glm::vec3 camPositions[3] = {
            {0.f, 6.5f, 43.f},
            {0.f, 5.f, 37.f},
            {0.f, 15.f, 38.f}
        };

        const glm::vec3 camTargets[3] = {
            {0.f, 22.f, -60.f},
            {0.f, 28.f, -42.f},
            {0.f, 18.f, -110.f}
        };

        orbit.reset(camPositions[0], camTargets[0]);
        bool homeWasPressed = false;

        glEnable(GL_CULL_FACE);

        float lastFrame =
            static_cast<float>(glfwGetTime());

        double lastTitleTime = glfwGetTime();
        int frames = 0;

        while (!glfwWindowShouldClose(window)) {
            double now = glfwGetTime();
            float time = static_cast<float>(now);

            float deltaTime = time - lastFrame;
            lastFrame = time;

            ++frames;

            if (now - lastTitleTime >= 0.5) {
                int fps = static_cast<int>(
                    frames / (now - lastTitleTime)
                );

                glfwSetWindowTitle(
                    window,
                    (
                        "Winter Astana by Night | FPS: " +
                        std::to_string(fps)
                    ).c_str()
                );

                frames = 0;
                lastTitleTime = now;
            }

            if (glfwGetKey(
                window, GLFW_KEY_ESCAPE
            ) == GLFW_PRESS) {
                glfwSetWindowShouldClose(
                    window, GLFW_TRUE
                );
            }

            // Start or restart Wow Moment with P in Winter City
            bool pPressed =
                glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;

            if (pPressed && !pWasPressed && activeScene == 0) {
                wowActive = true;
                orbit.reset(camPositions[0], camTargets[0]);
                wowStartTime = time;
                wowProgress = 0.0f;
                lightingEnabled = true;
            }

            pWasPressed = pPressed;

            if (wowActive) {
                wowProgress = std::clamp(
                    (time - wowStartTime) / wowDuration,
                    0.0f,
                    1.0f
                );
            }

            // Lighting toggle: L

            bool lPressed =
                glfwGetKey(window, GLFW_KEY_L)
                == GLFW_PRESS;

            if (lPressed && !lWasPressed) {
                lightingEnabled = !lightingEnabled;

                std::cout << "Point lighting: "
                          << (lightingEnabled ? "ON" : "OFF")
                          << std::endl;
            }

            lWasPressed = lPressed;

            bool fPressed =
                glfwGetKey(window, GLFW_KEY_F)
                == GLFW_PRESS;

            if (fPressed && !fWasPressed) {
                fireworksEnabled = !fireworksEnabled;

                std::cout << "Fireworks: "
                          << (fireworksEnabled ? "ON" : "OFF")
                          << std::endl;
            }

            fWasPressed = fPressed;

            bool nPressed =
                glfwGetKey(window, GLFW_KEY_N)
                == GLFW_PRESS;

            if (nPressed && !nWasPressed) {
                autoSnowEnabled = !autoSnowEnabled;

                std::cout << "Automatic snow: "
                          << (autoSnowEnabled ? "ON" : "OFF")
                          << std::endl;
            }

            nWasPressed = nPressed;

            bool leftPressed =
                glfwGetKey(window, GLFW_KEY_LEFT)
                == GLFW_PRESS;

            bool rightPressed =
                glfwGetKey(window, GLFW_KEY_RIGHT)
                == GLFW_PRESS;

            bool upPressed =
                glfwGetKey(window, GLFW_KEY_UP)
                == GLFW_PRESS;

            bool downPressed =
                glfwGetKey(window, GLFW_KEY_DOWN)
                == GLFW_PRESS;

            bool rPressed =
                glfwGetKey(window, GLFW_KEY_R)
                == GLFW_PRESS;

            bool windChanged = false;

            if (leftPressed && !leftWasPressed) {
                windDirection = -1.f;
                windStrength = std::max(
                    windStrength, 2.f
                );
                windChanged = true;
            }

            if (rightPressed && !rightWasPressed) {
                windDirection = 1.f;
                windStrength = std::max(
                    windStrength, 2.f
                );
                windChanged = true;
            }

            if (upPressed && !upWasPressed) {
                windStrength = std::min(
                    windStrength + 2.f, 18.f
                );
                windChanged = true;
            }

            if (downPressed && !downWasPressed) {
                windStrength = std::max(
                    windStrength - 2.f, 0.f
                );
                windChanged = true;
            }

            if (rPressed && !rWasPressed) {
                windDirection = 1.f;
                windStrength = 0.5f;
                windChanged = true;
            }

            leftWasPressed = leftPressed;
            rightWasPressed = rightPressed;
            upWasPressed = upPressed;
            downWasPressed = downPressed;
            rWasPressed = rPressed;

            if (windChanged) {
                float windX =
                    windDirection * windStrength;

                snow.setWind(windX, 0.f);

                std::cout
                    << "Wind X: " << windX
                    << " | Strength: " << windStrength
                    << std::endl;
            }

            int width, height;
            glfwGetFramebufferSize(
                window, &width, &height
            );

            if (width == 0 || height == 0) {
                glfwPollEvents();
                continue;
            }

            int requested = activeScene;
            bool choose = false;
            const bool menuWasOpen = menuOpen;
            if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) menuOpen = true;
            int windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            double cursorX, cursorY;
            glfwGetCursorPos(window, &cursorX, &cursorY);
            menu.update(time);
            int hovered = menu.pick(cursorX, cursorY, windowWidth, windowHeight);
            bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            if (menuOpen && mousePressed && !mouseWasPressed && hovered >= 0) {
                requested = hovered;
                choose = true;
            }
            mouseWasPressed = mousePressed;

            if (glfwGetKey(
                window, GLFW_KEY_1
            ) == GLFW_PRESS) {
                requested = 0;
                choose = true;
            }

            if (glfwGetKey(
                window, GLFW_KEY_2
            ) == GLFW_PRESS) {
                requested = 1;
                choose = true;
            }

            if (glfwGetKey(
                window, GLFW_KEY_3
            ) == GLFW_PRESS) {
                requested = 2;
                choose = true;
            }

            if (choose) menuOpen = false;
            if (menuWasOpen && !menuOpen) {
                music.playFromStart();
                } else if (!menuWasOpen && menuOpen) {
                    music.stopAndRewind();
                    }

            if (requested != activeScene) {
                activeScene = requested;

                camera.position =
                    camPositions[activeScene];

                camera.target =
                    camTargets[activeScene];
                wowActive = false;
                orbit.reset(camPositions[activeScene], camTargets[activeScene]);
            }

            if (menuOpen) {
                orbit.dragging = false;
                orbit.scroll = 0.0;
                glClearColor(0.005f, 0.012f, 0.03f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                menu.draw(shader, static_cast<float>(width) / height, hovered);
                glfwSwapBuffers(window);
                glfwPollEvents();
                continue;
            }

            // Orbit camera: mouse/touchpad drag, scroll, WASD, Home.
            bool homePressed = glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS;
            if (homePressed && !homeWasPressed) {
                orbit.reset(camPositions[activeScene], camTargets[activeScene]);
                wowActive = false;
            }
            homeWasPressed = homePressed;

            bool leftDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            if (leftDown && !orbit.dragging) {
                orbit.dragging = true;
                glfwGetCursorPos(window, &orbit.lastX, &orbit.lastY);
            } else if (!leftDown) {
                orbit.dragging = false;
            }

            if (orbit.dragging) {
                double x, y;
                glfwGetCursorPos(window, &x, &y);
                double dx = x - orbit.lastX;
                double dy = y - orbit.lastY;
                orbit.lastX = x;
                orbit.lastY = y;
                if (dx != 0.0 || dy != 0.0) {
                    orbit.yaw -= static_cast<float>(dx) * 0.005f;
                    orbit.pitch += static_cast<float>(dy) * 0.005f;
                    orbit.manual = true;
                }
            }

            float turnSpeed = 1.2f * std::min(deltaTime, 0.05f);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                orbit.yaw -= turnSpeed;
                orbit.manual = true;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                orbit.yaw += turnSpeed;
                orbit.manual = true;
            }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                orbit.pitch += turnSpeed;
                orbit.manual = true;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                orbit.pitch -= turnSpeed;
                orbit.manual = true;
            }
            if (orbit.scroll != 0.0) {
                orbit.distance *= std::pow(0.90f, static_cast<float>(orbit.scroll));
                orbit.scroll = 0.0;
                orbit.manual = true;
            }
            if (orbit.manual) wowActive = false;
            orbit.distance = std::clamp(orbit.distance, 12.f, 240.f);
            float minPitch = std::asin(std::clamp(
                (1.0f - orbit.target.y) / orbit.distance, -0.85f, 0.0f
            ));
            orbit.pitch = std::clamp(orbit.pitch, minPitch, 1.30f);

            // Keep EXPO's automatic camera until the user takes control.
            if (activeScene == 1 && !orbit.manual) {
                camera.position = camPositions[1];
                camera.position.x = std::sin(time * 0.25f) * 5.f;
                camera.target = camTargets[1];
            } else if (!wowActive || activeScene != 0) {
                orbit.apply(camera);
            }

            // Cinematic camera movement in Winter City
            if (wowActive && activeScene == 0) {
                float t = wowProgress;

                // Smooth ease-in/ease-out
                float smoothT = t * t * (3.0f - 2.0f * t);

                const glm::vec3 startPos(0.0f, 6.5f, 43.0f);
                const glm::vec3 endPos(0.0f, 85.0f, 100.0f);

                const glm::vec3 startTarget(0.0f, 22.0f, -60.0f);
                const glm::vec3 endTarget(0.0f, 12.0f, -55.0f);

                camera.position = glm::mix(
                    startPos, endPos, smoothT
                );

                camera.target = glm::mix(
                    startTarget, endTarget, smoothT
                );
            }

            glm::mat4 view = camera.view();

            glm::mat4 projection =
                camera.projection(
                    static_cast<float>(width) /
                    static_cast<float>(height)
                );

            glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT
            );

            glDisable(GL_DEPTH_TEST);

            skyShader.use();
            skyShader.setVec3(
                "uHorizon", skyHorizon[activeScene]
            );
            skyShader.setVec3(
                "uTop", skyTop[activeScene]
            );
            skyShader.setFloat(
                "uStars", skyStars[activeScene]
            );

            sky.draw(skyShader);

            glEnable(GL_DEPTH_TEST);

            shader.use();

            shader.setMat4("uView", view);
            shader.setMat4("uProj", projection);

            shader.setVec3(
                "uCamPos", camera.position
            );

            shader.setVec3(
                "uFogColor", fogColors[activeScene]
            );

            shader.setFloat(
                "uFogDensity", fogDensities[activeScene]
            );

            Lights::apply(
                shader,
                camera.position,
                time,
                activeScene,
                lightingEnabled
            );

            shader.setInt(
                "uLightingEnabled",
                lightingEnabled ? 1 : 0
            );

            shader.setInt(
                "uSceneIndex", activeScene
            );

            shader.setFloat("uTime", time);
            shader.setInt("uWowActive", wowActive ? 1 : 0);
            shader.setFloat("uWowProgress", wowProgress);

            shader.setInt(
                "uAutoSnowEnabled",
                autoSnowEnabled ? 1 : 0
            );

            if (activeScene == 0) {
                scene.update(time);
                scene.draw(shader);

            } else if (activeScene == 1) {
                expoScene.update(time);
                expoScene.draw(shader);

            } else {
                riverfrontScene.draw(shader);
            }

            snow.update(deltaTime);

            snow.draw(
                snowShader,
                view,
                projection,
                snowCounts[activeScene]
            );

            if (
                activeScene == 2 &&
                fireworksEnabled
            ) {
                fireworks.update(deltaTime);

                fireworks.draw(
                    fireworksShader,
                    view,
                    projection
                );
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";

        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

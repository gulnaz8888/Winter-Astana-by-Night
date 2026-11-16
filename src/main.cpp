
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "ExpoScene.h"
#include "RiverfrontScene.h"
#include "Sky.h"
#include "Lights.h"
#include "Snow.h"
#include "Fireworks.h"

static void onResize(
    GLFWwindow*,
    int width,
    int height
) {
    glViewport(0, 0, width, height);
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
        1280, 720,
        "Winter Astana by Night",
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

        Camera camera;
        camera.position = {0.f, 6.5f, 43.f};
        camera.target = {0.f, 22.f, -60.f};

        Snow snow(4000);
        Fireworks fireworks;

        int activeScene = 0;

        bool lightingEnabled = true;
        bool lWasPressed = false;

        bool fireworksEnabled = true;
        bool fWasPressed = false;

        // Wind settings.
        // 1 means wind to the right, -1 means left.
        float windDirection = 1.f;
        float windStrength = 0.5f;

        bool leftWasPressed = false;
        bool rightWasPressed = false;
        bool upWasPressed = false;
        bool downWasPressed = false;
        bool rWasPressed = false;

        snow.setWind(
            windDirection * windStrength,
            0.f
        );

        const int snowCounts[3] = {
            4000,  // Winter City
            4000,  // EXPO
            1000   // Riverfront
        };

        const glm::vec3 fogColors[3] = {
            {0.055f, 0.085f, 0.15f},
            {0.30f, 0.22f, 0.17f},
            {0.035f, 0.065f, 0.12f}
        };

        const float fogDensities[3] = {
            0.0023f,
            0.0025f,
            0.0018f
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

            if (
                glfwGetKey(window, GLFW_KEY_ESCAPE)
                == GLFW_PRESS
            ) {
                glfwSetWindowShouldClose(
                    window, GLFW_TRUE
                );
            }

            // Point lighting toggle: L.
            bool lPressed =
                glfwGetKey(window, GLFW_KEY_L)
                == GLFW_PRESS;

            if (lPressed && !lWasPressed) {
                lightingEnabled = !lightingEnabled;

                std::cout
                    << "Point lighting: "
                    << (lightingEnabled ? "ON" : "OFF")
                    << std::endl;
            }

            lWasPressed = lPressed;

            // Fireworks toggle: F.
            bool fPressed =
                glfwGetKey(window, GLFW_KEY_F)
                == GLFW_PRESS;

            if (fPressed && !fWasPressed) {
                fireworksEnabled = !fireworksEnabled;

                std::cout
                    << "Fireworks: "
                    << (fireworksEnabled ? "ON" : "OFF")
                    << std::endl;
            }

            fWasPressed = fPressed;

            // WIND CONTROLS

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

            // Change wind direction.
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

            // Increase wind strength.
            if (upPressed && !upWasPressed) {
                windStrength = std::min(
                    windStrength + 2.f,
                    18.f
                );
                windChanged = true;
            }

            // Decrease wind strength.
            if (downPressed && !downWasPressed) {
                windStrength = std::max(
                    windStrength - 2.f,
                    0.f
                );
                windChanged = true;
            }

            // Reset to original gentle drift.
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

            if (height == 0 || width == 0) {
                glfwPollEvents();
                continue;
            }

            // SCENE SWITCHING

            int requested = activeScene;

            if (
                glfwGetKey(window, GLFW_KEY_1)
                == GLFW_PRESS
            ) {
                requested = 0;
            }

            if (
                glfwGetKey(window, GLFW_KEY_2)
                == GLFW_PRESS
            ) {
                requested = 1;
            }

            if (
                glfwGetKey(window, GLFW_KEY_3)
                == GLFW_PRESS
            ) {
                requested = 2;
            }

            if (requested != activeScene) {
                activeScene = requested;

                camera.position =
                    camPositions[activeScene];

                camera.target =
                    camTargets[activeScene];
            }

            if (activeScene == 1) {
                camera.position.x =
                    std::sin(time * 0.25f) * 5.f;
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

            // SKY

            glDisable(GL_DEPTH_TEST);

            skyShader.use();

            skyShader.setVec3(
                "uHorizon",
                skyHorizon[activeScene]
            );

            skyShader.setVec3(
                "uTop",
                skyTop[activeScene]
            );

            skyShader.setFloat(
                "uStars",
                skyStars[activeScene]
            );

            sky.draw(skyShader);

            glEnable(GL_DEPTH_TEST);

            // LIGHTING AND SCENE SHADER

            shader.use();

            shader.setMat4("uView", view);
            shader.setMat4("uProj", projection);

            shader.setVec3(
                "uCamPos",
                camera.position
            );

            shader.setVec3(
                "uFogColor",
                fogColors[activeScene]
            );

            shader.setFloat(
                "uFogDensity",
                fogDensities[activeScene]
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
                "uSceneIndex",
                activeScene
            );

            shader.setFloat("uTime", time);

            // DRAW SELECTED SCENE

            if (activeScene == 0) {
                scene.update(time);
                scene.draw(shader);

            } else if (activeScene == 1) {
                expoScene.update(time);
                expoScene.draw(shader);

            } else {
                riverfrontScene.draw(shader);
            }

            // SNOW PARTICLES

            snow.update(deltaTime);

            snow.draw(
                snowShader,
                view,
                projection,
                snowCounts[activeScene]
            );

            // FIREWORKS ONLY IN RIVERFRONT

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

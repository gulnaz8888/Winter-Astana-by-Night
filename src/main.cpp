#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>
#include <string>
#include "Shader.h"
#include "Camera.h"

static void onResize(GLFWwindow*, int width, int height) {
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

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Winter Astana by Night", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n";
        glfwTerminate();
        return 1;
    }
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSwapInterval(1);
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";

    try {
        Shader shader("shaders/basic.vert", "shaders/basic.frag");
        Camera camera;
        glEnable(GL_DEPTH_TEST);

        double lastTitleTime = glfwGetTime();
        int frames = 0;

        while (!glfwWindowShouldClose(window)) {
            double now = glfwGetTime();
            ++frames;
            if (now - lastTitleTime >= 0.5) {
                int fps = (int)(frames / (now - lastTitleTime));
                glfwSetWindowTitle(window, ("Winter Astana by Night | FPS: " + std::to_string(fps)).c_str());
                frames = 0;
                lastTitleTime = now;
            }
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            if (height == 0) {
                glfwPollEvents();
                continue;
            }

            glClearColor(0.02f, 0.03f, 0.10f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            shader.setMat4("uView", camera.view());
            shader.setMat4("uProj", camera.projection((float)width / height));

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        glfwTerminate();
        return 1;
    }

    glfwTerminate();
    return 0;
}

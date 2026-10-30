#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return 1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* w = glfwCreateWindow(1280, 720, "Winter Astana by Night", nullptr, nullptr);
    if (!w) { std::cerr << "Window failed\n"; glfwTerminate(); return 1; }
    glfwMakeContextCurrent(w);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cerr << "GLAD failed\n"; return 1; }
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";

    while (!glfwWindowShouldClose(w)) {
        glClearColor(0.02f, 0.03f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(w);
        glfwPollEvents();
    }
    glfwTerminate();
}

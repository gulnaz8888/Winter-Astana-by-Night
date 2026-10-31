#include "Sky.h"

Sky::Sky() {
    glGenVertexArrays(1, &vao);
}

Sky::~Sky() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
}

void Sky::draw(const Shader& shader) const {
    shader.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

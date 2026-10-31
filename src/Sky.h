#pragma once
#include <glad/glad.h>
#include "Shader.h"

class Sky {
public:
    Sky();
    ~Sky();
    Sky(const Sky&) = delete;
    Sky& operator=(const Sky&) = delete;

    void draw(const Shader& shader) const;

private:
    GLuint vao = 0;
};

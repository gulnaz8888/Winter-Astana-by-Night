#pragma once
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned>& indices);
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void draw() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei count = 0;
    void release();
};

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

namespace {

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Cannot open shader file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compile(GLenum type, const std::string& path) {
    std::string source = readFile(path);
    const char* code = source.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error("Shader compile error in " + path + ":\n" + log);
    }
    return shader;
}

}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vs = compile(GL_VERTEX_SHADER, vertexPath);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentPath);
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(program, 1024, nullptr, log);
        throw std::runtime_error(std::string("Shader link error:\n") + log);
    }
}

Shader::~Shader() {
    if (program) {
        glDeleteProgram(program);
    }
}

void Shader::use() const {
    glUseProgram(program);
}

GLint Shader::location(const std::string& name) const {
    auto it = locations.find(name);
    if (it != locations.end()) {
        return it->second;
    }
    GLint loc = glGetUniformLocation(program, name.c_str());
    locations[name] = loc;
    return loc;
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(location(name), 1, glm::value_ptr(value));
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(location(name), value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(location(name), value);
}

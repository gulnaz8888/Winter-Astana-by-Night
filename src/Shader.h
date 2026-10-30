#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;
    GLuint id() const { return program; }
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

private:
    GLuint program = 0;
    mutable std::unordered_map<std::string, GLint> locations;
    GLint location(const std::string& name) const;
};

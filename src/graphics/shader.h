#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <vector>

namespace ITD
{
    struct Uniform
    {
        std::string name;
        GLenum type;
    };

    class Shader
    {
    private:
        GLuint m_id;
        std::vector<Uniform> m_uniforms;
        std::unordered_map<std::string, GLint> m_locations;

    public:
        Shader(const std::string& vert_str, const std::string& frag_str);
        ~Shader();

        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;

        GLuint id() const;

        const std::vector<Uniform>& uniforms() const;
        GLint uniform_location(const std::string& name) const;
    };
}

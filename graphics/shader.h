#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm/mat4x4.hpp>

namespace Uboat
{
    class Shader
    {
    private:
        GLuint m_id;
        std::unordered_map<std::string, GLint> m_uniform_cache;

    public:
        Shader(const std::string& vert_str, const std::string& frag_str);
        ~Shader();

        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;

        GLuint id() const;

        void set_uniform_1i(const GLchar* name, int value);
        void set_uniform_mat4(const GLchar* name, const glm::mat4& value);

    private:
        GLint uniform_location(const GLchar* name);
    };
}

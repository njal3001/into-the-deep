#include "shader.h"
#include <assert.h>

namespace Uboat
{
    Shader::Shader(const std::string& vert_str, const std::string& frag_str)
        : m_id(0)
    {
        GLuint vert = glCreateShader(GL_VERTEX_SHADER);
        GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vert_src = vert_str.c_str();
        const char* frag_src = frag_str.c_str();

        GLchar log[1024];
        GLsizei log_length = 0;

        glShaderSource(vert, 1, &vert_src, NULL);
        glCompileShader(vert);

        // Check if vertex shader compilation failed
        glGetShaderInfoLog(vert, 1024, &log_length, log);
        if (log_length)
        {
            glDeleteShader(vert);
            printf("%s", log);
            assert(false);
        }

        glShaderSource(frag, 1, &frag_src, NULL);
        glCompileShader(frag);

        // Check if fragment shader compilation failed
        glGetShaderInfoLog(frag, 1024, &log_length, log);
        if (log_length)
        {
            glDeleteShader(vert);
            glDeleteShader(frag);
            printf("%s", log);
            assert(false);
        }

        m_id = glCreateProgram();
        glAttachShader(m_id, vert);
        glAttachShader(m_id, frag);

        glLinkProgram(m_id);
        glGetProgramInfoLog(m_id, 1024, &log_length, log);

        glDetachShader(m_id, vert);
        glDetachShader(m_id, frag);
        glDeleteShader(vert);
        glDeleteShader(frag);


        // Check if shader linking failed
        if (log_length)
        {
            printf("%s", log);
            assert(false);
        }
    }

    Shader::~Shader()
    {
        if (m_id)
        {
            glDeleteProgram(m_id);
        }
    }

    GLuint Shader::id() const
    {
        return m_id;
    }

    void Shader::set_uniform_1i(const GLchar* name, int value)
    {
        glUniform1i(uniform_location(name), value);
    }

    void Shader::set_uniform_mat4(const GLchar* name, const glm::mat4& value)
    {
        glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, &value[0][0]);
    }

    GLint Shader::uniform_location(const GLchar* name)
    {
        if (m_uniform_cache.find(name) != m_uniform_cache.end())
        {
            return m_uniform_cache[name];
        }

        GLint location = glGetUniformLocation(m_id, name);
        m_uniform_cache[name] = location;

        return location;
    }
}

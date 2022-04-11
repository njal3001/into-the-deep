#include "shader.h"
#include <assert.h>

namespace ITD {

Shader::Shader(const std::string &vert_str, const std::string &frag_str)
    : m_id(0)
{
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

    const char *vert_src = vert_str.c_str();
    const char *frag_src = frag_str.c_str();

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

    // Get uniforms
    size_t max_name_length = 256;
    GLint active_uniforms;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &active_uniforms);

    for (int i = 0; i < active_uniforms; i++)
    {
        GLsizei length;
        GLsizei size;
        GLenum type;
        GLchar name[max_name_length + 1];

        glGetActiveUniform(m_id, i, max_name_length, &length, &size, &type,
                           name);
        name[length] = '\0';

        m_uniforms.push_back({
            .name = std::string(name),
            .type = type,
        });

        m_locations[m_uniforms.back().name] = glGetUniformLocation(m_id, name);
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

const std::vector<Uniform> &Shader::uniforms() const
{
    return m_uniforms;
}

GLint Shader::uniform_location(const std::string &name) const
{
    return m_locations.at(name);
}

}  // namespace ITD

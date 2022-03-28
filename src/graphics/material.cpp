#include "material.h"
#include <string.h>

namespace ITD
{
    Material::Material()
        : m_shader(nullptr)
    {}

    Material::Material(const Shader *shader)
    {
        set_shader(shader);
    }

    void Material::set_shader(const Shader *shader)
    {
        m_shader = shader;
        m_textures.clear();

        if (!shader) return;

        const auto& uniforms = shader->uniforms();

        size_t values_size = 0;
        for (auto& uniform : uniforms)
        {
            if (uniform.type == GL_SAMPLER_2D)
            {
                m_textures.push_back(nullptr);
            }
            else
            {
                values_size += uniform_length(uniform);
            }
        }

        m_values.resize(values_size);
    }

    const Shader* Material::shader() const
    {
        return m_shader;
    }

    bool Material::set_texture(const std::string& name, const Texture* texture, const size_t slot)
    {
        const auto& uniforms = m_shader->uniforms();

        for (auto& uniform : uniforms)
        {
            if (uniform.type != GL_SAMPLER_2D) continue;

            if (name == uniform.name)
            {
                m_textures[slot] = texture;
                return true;
            }
        }

        return false;
    }

    bool Material::set_value(const std::string& name, const float *value)
    {
        const auto& uniforms = m_shader->uniforms();

        size_t offset = 0;
        for (auto& uniform : uniforms)
        {
            if (uniform.type == GL_SAMPLER_2D) continue;

            const size_t length = uniform_length(uniform);

            if (name == uniform.name)
            {
                memcpy(&m_values[offset], value, length * sizeof(float));
                return true;
            }

            offset += length;
        }

        return false;
    }

    const Texture* Material::get_texture(const size_t slot) const
    {
        return m_textures[slot];
    }

    const std::vector<float>& Material::get_values() const
    {
        return m_values;
    }

    size_t Material::uniform_length(const Uniform& uniform) const
    {
        switch (uniform.type)
        {
            case GL_FLOAT:
                return 1;
            case GL_FLOAT_VEC2:
                return 2;
            case GL_FLOAT_MAT4:
                return 16;
        };

        return 0;
    }
}

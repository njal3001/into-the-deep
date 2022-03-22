#pragma once
#include "shader.h"
#include "texture.h"

namespace ITD
{
    class Material
    {
    private:
        const Shader *m_shader;
        std::vector<const Texture*> m_textures;
        std::vector<float> m_values;

    public:
        Material();
        Material(const Shader *shader);

        Material(const Material& other) = delete;
        Material& operator=(const Material& other) = delete;

        void set_shader(const Shader *shader);
        const Shader* shader() const;

        bool set_texture(const std::string& name, const Texture* texture, const size_t slot = 0);
        bool set_value(const std::string& name, const float *value);

        const Texture* get_texture(const size_t slot) const;

        const std::vector<float>& get_values() const;

    private:
        size_t uniform_length(const Uniform& uniform) const;
    };
}

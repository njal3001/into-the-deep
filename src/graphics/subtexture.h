#pragma once
#include <array>
#include "../maths/shapes.h"
#include "texture.h"

namespace ITD {

class Subtexture
{
private:
    const Texture *m_texture;
    Recti m_bounds;
    std::array<glm::vec2, 4> m_tex_coords;

public:
    Subtexture();
    Subtexture(const Texture *texture, const Recti &bounds);

    void set_texture(const Texture *texture);
    void set_bounds(const Recti &bounds);
    const Texture *get_texture() const;
    Recti get_bounds() const;
    const std::array<glm::vec2, 4> &get_tex_coords() const;

private:
    void update_tex_coords();
};

}  // namespace ITD

#include "subtexture.h"
#include <assert.h>

namespace ITD {

Subtexture::Subtexture()
    : m_texture(nullptr)
{
}

Subtexture::Subtexture(const Texture *texture, const Recti &bounds)
    : m_texture(texture)
    , m_bounds(bounds)
{
    update_tex_coords();
}

void Subtexture::set_texture(const Texture *texture)
{
    m_texture = texture;
    update_tex_coords();
}

void Subtexture::set_bounds(const Recti &bounds)
{
    m_bounds = bounds;

    if (m_texture)
    {
        update_tex_coords();
    }
}

const Texture *Subtexture::get_texture() const
{
    return m_texture;
}

Recti Subtexture::get_bounds() const
{
    return m_bounds;
}

const std::array<glm::vec2, 4> &Subtexture::get_tex_coords() const
{
    return m_tex_coords;
}

void Subtexture::update_tex_coords()
{
    const float tex_w = m_texture->width();
    const float tex_h = m_texture->height();

    // Check bounds
    assert(m_bounds.bl.x >= 0 && m_bounds.tr.x <= tex_w && m_bounds.bl.y >= 0 &&
           m_bounds.tr.y <= tex_h);

    const float sx = 1.0f / tex_w;
    const float sy = 1.0f / tex_h;

    m_tex_coords[0] = glm::vec2(m_bounds.bl.x * sx, m_bounds.bl.y * sy);
    m_tex_coords[1] =
        glm::vec2(m_bounds.bl.x * sx, (m_bounds.tr.y + 1.0f) * sy);
    m_tex_coords[2] =
        glm::vec2((m_bounds.tr.x + 1.0f) * sx, (m_bounds.tr.y + 1.0f) * sy);
    m_tex_coords[3] =
        glm::vec2((m_bounds.tr.x + 1.0f) * sx, m_bounds.bl.y * sy);
}

}  // namespace ITD

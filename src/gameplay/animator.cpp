#include "animator.h"
#include "../platform.h"
#include "../graphics/subtexture.h"

namespace ITD
{
    Animator::Animator(const std::string &sprite_sheet, const Recti &frame_bounds, 
            const size_t nframes, const float frame_length, 
            const glm::vec2 &offset, const glm::vec2 &pivot)
        : m_sprite_sheet(sprite_sheet), m_frame_bounds(frame_bounds), 
        m_nframes(nframes), frame_length(frame_length), 
        m_frame_index(0), m_frame_timer(0.0f), offset(offset), 
        pivot(pivot), rotation(0.0f)
    {
        const std::string path = Platform::app_path() + "../res/" + sprite_sheet + ".png"; 
        Image img(path);
        m_texture.load(img);

        m_subtexture.set_bounds(m_frame_bounds);
        m_subtexture.set_texture(&m_texture);
    }

    void Animator::update(const float elapsed)
    {
        m_frame_timer += elapsed;
        if (m_frame_timer >= frame_length)
        {
            m_frame_index = (m_frame_index + 1) % m_nframes;
            m_frame_timer = 0.0f;

            m_subtexture.set_bounds(m_frame_bounds +
                    glm::vec2(m_frame_index * m_frame_bounds.width(), 0.0f));
        }
    }

    void Animator::render(Renderer *renderer)
    {
        const glm::vec2 pos = m_entity->get_pos() + offset;

        const glm::mat4 mat = Calc::rotate(rotation, pos + pivot);
        renderer->push_matrix(mat);
        renderer->tex(m_subtexture, pos, Color::white);
        renderer->pop_matrix();
    }
}

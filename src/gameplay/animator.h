#pragma once

#include "../graphics/texture.h"
#include "ecs.h"

namespace ITD {

class Animator : public Component
{
public:
    float frame_length;
    glm::vec2 offset;
    glm::vec2 pivot;
    float rotation;

private:
    std::string m_sprite_sheet;
    Texture m_texture;
    Recti m_frame_bounds;
    size_t m_nframes;
    size_t m_frame_index;
    float m_frame_timer;
    Subtexture m_subtexture;

public:
    Animator(const std::string &sprite_sheet, const Recti &frame_bounds,
             size_t nframes, float frame_length, const glm::vec2 &offset,
             const glm::vec2 &pivot);

    void update(float elapsed) override;
    void render(Renderer *renderer) override;
};

}  // namespace ITD

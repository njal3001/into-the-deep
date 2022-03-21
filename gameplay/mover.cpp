#include "mover.h"

namespace Uboat
{
    Mover::Mover()
        : collider(nullptr), vel(glm::vec2()), collides_with(Mask::Solid), on_hit(nullptr)
    {}

    void Mover::update(const float elapsed)
    {
        m_entity->pos += vel * elapsed;
        collider->invalidate_cache();
    }
}

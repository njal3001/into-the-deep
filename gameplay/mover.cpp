#include "mover.h"

namespace Uboat
{
    Mover::Mover()
        : collider(nullptr), stop_mask(Mask::Solid)
    {}

    void Mover::update(const float elapsed)
    {
        m_entity->pos += vel * elapsed;
    }
}

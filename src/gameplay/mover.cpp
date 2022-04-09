#include "mover.h"

namespace ITD {

Mover::Mover()
    : vel(glm::vec2())
{
}

void Mover::update(const float elapsed)
{
    m_entity->translate(vel * elapsed);
}

}  // namespace ITD

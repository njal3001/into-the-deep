#include "mover.h"

namespace ITD {

Mover::Mover()
    : vel(glm::vec2())
{
}

void Mover::update(float elapsed)
{
    m_entity->translate(vel * elapsed);
}

}  // namespace ITD

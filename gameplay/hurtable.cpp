#include "hurtable.h"

namespace Uboat
{
    void Hurtable::hurt(const glm::vec2 &dir)
    {
        health--;

        if (on_hurt)
        {
            on_hurt(this, dir);
        }

        if (health <= 0)
        {
            m_entity->destroy();
        }
    }
}

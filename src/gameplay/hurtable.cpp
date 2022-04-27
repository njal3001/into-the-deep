#include "hurtable.h"

namespace ITD {

Hurtable::Hurtable()
    : m_flicker_timer(0.0f)
    , flicker_time(0.5f)
    , invincible(false)
{
}

bool Hurtable::hurt(const glm::vec2 &dir)
{
    if (!invincible  && m_flicker_timer <= 0.0f)
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

        m_flicker_timer = flicker_time;

        return true;
    }

    return false;
}

void Hurtable::update(float elapsed)
{
    m_flicker_timer -= elapsed;
}

}  // namespace ITD

#include "hurtable.h"

namespace ITD
{

    Hurtable::Hurtable()
        : m_invincible_timer(0.0f), invincible_time(0.5f)
    {}

    bool Hurtable::hurt(const glm::vec2 &dir)
    {
        if (m_invincible_timer <= 0.0f)
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

            m_invincible_timer = invincible_time;

            return true;
        }

        return false;
    }

    void Hurtable::update(const float elapsed)
    {
        m_invincible_timer -= elapsed;
    }
}

#include "hurtable.h"

namespace Uboat
{
    void Hurtable::hurt()
    {
        health--;
        if (health <= 0)
        {
            m_entity->destroy();
        }
    }
}

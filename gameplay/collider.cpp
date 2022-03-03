#include "collider.h"
#include "../platform.h"

namespace Uboat
{
    Collider::Collider(const Rectf& bounds, const bool dynamic, const float rotation)
        : bounds(bounds), dynamic(dynamic), rotation(rotation), m_timestamp(0)
    {
        if (!dynamic)
        {
            recalculate();
        }
    }

    Quadf Collider::get()
    {
        const uint64_t ticks = Platform::ticks();
        if (ticks != m_timestamp && dynamic)
        {
            recalculate();
            m_timestamp = ticks;
        }

        return m_cached;
    }

    void Collider::invalidate_cache()
    {
        m_timestamp = 0;
    }

    void Collider::recalculate()
    {
        m_cached = Quadf(bounds, rotation);
        m_cached.offset(m_entity->pos);
    }
}

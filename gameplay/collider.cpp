#include "collider.h"
#include "../platform.h"
#include <algorithm>

namespace Uboat
{
    Collider::Collider(const Rectf& bounds, const bool dynamic, const float rotation)
        : bounds(bounds), dynamic(dynamic), rotation(rotation),
        m_timestamp(-1)
    {}

    void Collider::awake()
    {
        if (!dynamic)
        {
            recalculate();
        }
    }

    void Collider::refresh()
    {
        const int ticks = Platform::ticks();
        if (ticks != m_timestamp && dynamic)
        {
            recalculate();
            m_timestamp = ticks;
        }
    }

    const Quadf& Collider::get()
    {
        refresh();
        return m_cached;
    }

    const Collider::Axes& Collider::axes()
    {
        refresh();
        return m_axes;
    }

    void Collider::invalidate_cache()
    {
        m_timestamp = -1;
    }

    Collider::Projection Collider::project(const glm::vec2& axis) const
    {
        const float p0 = glm::dot(m_cached.a, axis);
        const float p1 = glm::dot(m_cached.b, axis);
        const float p2 = glm::dot(m_cached.c, axis);
        const float p3 = glm::dot(m_cached.d, axis);

        std::initializer_list<float> all = { p0, p1, p2, p3 };

        return
        {
            .start = std::min(all),
            .end = std::max(all)
        };
    }

    bool Collider::overlaps(Collider& other)
    {
        return push_out(other) != glm::vec2();
    }

    glm::vec2 Collider::push_out(Collider& other)
    {
        const Axes* all_axes[2] = { &axes(), &other.axes() };

        // No need to check both colliders axes if the rotation is the same
        const size_t naxes = 2 - (rotation == other.rotation);

        float min_push = FLT_MAX;
        glm::vec2 push_dir;

        for (size_t i = 0; i < naxes; i++)
        {
            const Axes *axes = all_axes[i];

            for (const auto& axis : { axes->ax1, axes->ax2 })
            {
                const Projection prj1 = project(axis);
                const Projection prj2 = other.project(axis);

                // No push out if not overlapping
                if (std::min(prj1.end, prj2.end) < std::max(prj1.start, prj2.start))
                {
                    return glm::vec2();
                }

                // Check if positive or negative direction push is smallest
                const float push1 = prj2.end - prj1.start;
                const float push2 = prj2.start - prj1.end;
                const float push = std::abs(push1) < std::abs(push2) ? push1 : push2;

                if (std::abs(push) < std::abs(min_push))
                {
                    min_push = push;
                    push_dir = axis;
                }
            }
        }

        return push_dir * min_push;
    }

    void Collider::recalculate()
    {
        m_cached = Quadf(bounds, rotation);
        m_cached.offset(m_entity->pos);

        m_axes.ax1 = glm::normalize(m_cached.d - m_cached.a);
        m_axes.ax2 = glm::normalize(m_cached.b - m_cached.a);
    }
    
    void Collider::render(Renderer *renderer)
    {
        renderer->quad(m_cached.a, m_cached.b, m_cached.c, m_cached.d, Color::red);
    }
}

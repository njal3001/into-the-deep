#include "collider.h"
#include "../platform.h"
#include "mover.h"
#include <algorithm>

namespace ITD
{
    Collider::Collider(const Rectf &bounds, const float rotation)
        : bounds(bounds), rotation(rotation), dynamic(true), mask(Mask::None),
        m_in_bucket(false), m_timestamp(-1)
    {}

    void Collider::awake()
    {
        Mover *mover = get<Mover>();
        if (!mover)
        {
            dynamic = false;
            recalculate();
            scene()->collision_handler()->update_buckets(this);
        }
    }

    void Collider::on_removed()
    {
        scene()->collision_handler()->remove(this);
    }

    void Collider::refresh()
    {
        const int ticks = Platform::frame();
        if (ticks != m_timestamp && dynamic)
        {
            m_timestamp = ticks;
            recalculate();
        }
    }

    const Quadf &Collider::quad()
    {
        refresh();
        return m_quad;
    }

    const Collider::Axes &Collider::axes()
    {
        refresh();
        return m_axes;
    }

    const Rectf &Collider::bbox()
    {
        refresh();
        return m_bbox;
    }

    void Collider::invalidate_cache()
    {
        m_timestamp = -1;
    }

    Collider::Projection Collider::project(const glm::vec2 &axis) const
    {
        const float p0 = glm::dot(m_quad.a, axis);
        const float p1 = glm::dot(m_quad.b, axis);
        const float p2 = glm::dot(m_quad.c, axis);
        const float p3 = glm::dot(m_quad.d, axis);

        std::initializer_list<float> all = { p0, p1, p2, p3 };

        return
        {
            .start = std::min(all),
            .end = std::max(all)
        };
    }

    bool Collider::overlaps(Collider &other)
    {
        return push_out(other) != glm::vec2();
    }

    glm::vec2 Collider::push_out(Collider &other)
    {
        const Axes* all_axes[2] = { &axes(), &other.axes() };

        // No need to check both colliders axes if the rotation is the same
        const size_t naxes = 2 - (rotation == other.rotation);

        float min_push = FLT_MAX;
        glm::vec2 push_dir;

        for (size_t i = 0; i < naxes; i++)
        {
            const Axes *axes = all_axes[i];

            for (const auto &axis : { axes->ax1, axes->ax2 })
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

    float Collider::distance(Collider &other)
    {
        const Axes* all_axes[2] = { &axes(), &other.axes() };

        // No need to check both colliders axes if the rotation is the same
        const size_t naxes = 2 - (rotation == other.rotation);

        float max_dist = 0.0f;

        for (size_t i = 0; i < naxes; i++)
        {
            const Axes *axes = all_axes[i];

            for (const auto &axis : { axes->ax1, axes->ax2 })
            {
                const Projection prj1 = project(axis);
                const Projection prj2 = other.project(axis);

                const float dist = std::max(prj1.start, prj2.start) - std::min(prj1.end, prj2.end);
                max_dist = std::max(dist, max_dist);
            }
        }

        return max_dist;
    }

    Collider *Collider::check(const uint32_t mask)
    {
        return scene()->collision_handler()->check(this, mask);
    }

    void Collider::check_all(const uint32_t mask, std::vector<Collider*> *out)
    {
        scene()->collision_handler()->check_all(this, mask, out);
    }

    void Collider::recalculate()
    {
        m_quad = Quadf(bounds, rotation);
        m_quad.offset(m_entity->pos);

        m_axes.ax1 = glm::normalize(m_quad.d - m_quad.a);
        m_axes.ax2 = glm::normalize(m_quad.b - m_quad.a);

        float max_x = m_quad.a.x;
        max_x = std::max(m_quad.b.x, max_x);
        max_x = std::max(m_quad.c.x, max_x);
        max_x = std::max(m_quad.d.x, max_x);

        float min_x = m_quad.a.x;
        min_x = std::min(m_quad.b.x, min_x);
        min_x = std::min(m_quad.c.x, min_x);
        min_x = std::min(m_quad.d.x, min_x);

        float max_y = m_quad.a.y;
        max_y = std::max(m_quad.b.y, max_y);
        max_y = std::max(m_quad.c.y, max_y);
        max_y = std::max(m_quad.d.y, max_y);

        float min_y = m_quad.a.y;
        min_y = std::min(m_quad.b.y, min_y);
        min_y = std::min(m_quad.c.y, min_y);
        min_y = std::min(m_quad.d.y, min_y);

        m_bbox.bl = glm::vec2(min_x, min_y);
        m_bbox.tr = glm::vec2(max_x, max_y);
    }
    
    void Collider::render(Renderer *renderer)
    {
        renderer->quad(m_quad.a, m_quad.b, m_quad.c, m_quad.d, Color::red);
    }
}

#include "collider.h"
#include "../platform.h"
#include "mover.h"
#include <algorithm>

namespace ITD
{
    Collider::Collider(const Rectf &bounds, const float rotation, const bool dynamic)
        : m_bounds(bounds), m_rotation(rotation), m_dynamic(dynamic), mask(Mask::None), active(true),
        trigger_only(false), m_in_bucket(false), m_invalid_cache(true)
    {}

    void Collider::awake()
    {
        recalculate();
        scene()->collision_handler()->update_buckets(this);
        m_invalid_cache = false;

        if (m_dynamic)
        {
            scene()->collision_handler()->register_dynamic(this);
        }
    }

    void Collider::on_removed()
    {
        if (m_dynamic)
        {
            scene()->collision_handler()->deregister_dynamic(this);
        }

        scene()->collision_handler()->remove(this);
    }

    void Collider::refresh()
    {
        if (m_invalid_cache)
        {
            recalculate();
            m_invalid_cache = false;

            if (!m_dynamic)
            {
                scene()->collision_handler()->update_buckets(this);
            }
        }
    }

    void Collider::set_bounds(const Rectf &bounds)
    {
        m_bounds = bounds;
        m_invalid_cache = true;
    }

    Rectf Collider::get_bounds() const
    {
        return m_bounds;
    }

    void Collider::set_rotation(const float rotation)
    {
        m_rotation = rotation;
        m_invalid_cache = true;
    }

    void Collider::rotate(const float amount)
    {
        m_rotation += amount;
        m_invalid_cache = true;
    }

    float Collider::get_rotation() const
    {
        return m_rotation;
    }

    void Collider::set_dynamic(const bool dynamic)
    {
        if (dynamic != m_dynamic)
        {
            m_dynamic = dynamic;

            if (dynamic)
            {
                scene()->collision_handler()->register_dynamic(this);
            }
            else
            {
                scene()->collision_handler()->deregister_dynamic(this);
            }
        }
    }

    bool Collider::is_dynamic() const
    {
        return m_dynamic;
    }

    Quadf Collider::quad()
    {
        refresh();
        return m_quad;
    }

    Rectf Collider::bbox()
    {
        refresh();
        return m_bbox;
    }

    void Collider::on_position_changed()
    {
        m_invalid_cache = true;
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
        refresh();
        other.refresh();

        const glm::vec2* all_axes[2] = { m_axes, other.m_axes };

        // No need to check both colliders axes if the rotation is the same
        const size_t naxes = 2 - (m_rotation == other.m_rotation);

        float min_push = FLT_MAX;
        glm::vec2 push_dir;

        for (size_t i = 0; i < naxes; i++)
        {
            const glm::vec2 *axes = all_axes[i];

            for (size_t j = 0; j < 2; j++)
            {
                const glm::vec2 axis = axes[j];

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
        refresh();
        other.refresh();

        const glm::vec2* all_axes[2] = { m_axes, other.m_axes };

        // No need to check both colliders axes if the rotation is the same
        const size_t naxes = 2 - (m_rotation == other.m_rotation);

        float max_dist = 0.0f;

        for (size_t i = 0; i < naxes; i++)
        {
            const glm::vec2 *axes = all_axes[i];

            for (size_t j = 0; j < 2; j++)
            {
                const glm::vec2 axis = axes[j];

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
        m_quad = Quadf(m_bounds, m_rotation);
        m_quad += m_entity->get_pos();

        m_axes[0] = glm::normalize(m_quad.d - m_quad.a);
        m_axes[1] = glm::normalize(m_quad.b - m_quad.a);

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

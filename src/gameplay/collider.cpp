#include "collider.h"
#include <algorithm>
#include <glm/gtx/vector_angle.hpp>
#include "../platform.h"
#include "mover.h"

namespace ITD {

Collider::Collider(const Rectf &bounds, float rotation, bool dynamic)
    : m_bounds(bounds)
    , m_rotation(rotation)
    , m_dynamic(dynamic)
    , mask(Mask::None)
    , collides_with(Mask::None)
    , active(true)
    , trigger_only(false)
    , m_in_bucket(false)
    , m_invalid_cache(true)
{
}

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
    Component::on_removed();

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

void Collider::set_rotation(float rotation)
{
    m_rotation = rotation;
    m_invalid_cache = true;
}

void Collider::rotate(float amount)
{
    m_rotation += amount;
    m_invalid_cache = true;
}

float Collider::get_rotation() const
{
    return m_rotation;
}

void Collider::face_towards(const glm::vec2 &dir)
{
    m_rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), Calc::right);
}

void Collider::set_dynamic(bool dynamic)
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
    float min = FLT_MAX;
    float max = -FLT_MAX;

    for (const auto &point : m_quad.values)
    {
        float dot = glm::dot(point, axis);
        min = std::min(dot, min);
        max = std::max(dot, max);
    }

    return {.start = min, .end = max};
}

bool Collider::overlaps(Collider &other)
{
    return push_out(other) != glm::vec2();
}

glm::vec2 Collider::push_out(Collider &other)
{
    refresh();
    other.refresh();

    const glm::vec2 *all_axes[2] = {m_axes, other.m_axes};

    // No need to check both colliders axes if the rotation is the same
    size_t naxes = 2 - (m_rotation == other.m_rotation);

    float min_push = FLT_MAX;
    glm::vec2 push_dir;

    for (size_t i = 0; i < naxes; i++)
    {
        const glm::vec2 *axes = all_axes[i];

        for (size_t j = 0; j < 2; j++)
        {
            glm::vec2 axis = axes[j];

            Projection prj1 = project(axis);
            Projection prj2 = other.project(axis);

            // No push out if not overlapping
            if (std::min(prj1.end, prj2.end) < std::max(prj1.start, prj2.start))
            {
                return glm::vec2();
            }

            // Check if positive or negative direction push is smallest
            float push1 = prj2.end - prj1.start;
            float push2 = prj2.start - prj1.end;
            float push = std::abs(push1) < std::abs(push2) ? push1 : push2;

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

    const glm::vec2 *all_axes[2] = {m_axes, other.m_axes};

    // No need to check both colliders axes if the rotation is the same
    size_t naxes = 2 - (m_rotation == other.m_rotation);

    float max_dist = 0.0f;

    for (size_t i = 0; i < naxes; i++)
    {
        const glm::vec2 *axes = all_axes[i];

        for (size_t j = 0; j < 2; j++)
        {
            glm::vec2 axis = axes[j];

            Projection prj1 = project(axis);
            Projection prj2 = other.project(axis);

            float dist =
                std::max(prj1.start, prj2.start) - std::min(prj1.end, prj2.end);
            max_dist = std::max(dist, max_dist);
        }
    }

    return max_dist;
}

Collider *Collider::check(uint32_t mask)
{
    return scene()->collision_handler()->check(this, mask);
}

void Collider::check_all(uint32_t mask, std::vector<Collider *> *out)
{
    scene()->collision_handler()->check_all(this, mask, out);
}

void Collider::recalculate()
{
    m_quad = Quadf(m_bounds, m_rotation);
    m_quad += m_entity->get_pos();

    m_axes[0] = Calc::normalize(m_quad.d - m_quad.a);
    m_axes[1] = Calc::normalize(m_quad.b - m_quad.a);

    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float max_x = -FLT_MAX;
    float max_y = -FLT_MAX;

    for (const auto &point : m_quad.values)
    {
        min_x = std::min(point.x, min_x);
        min_y = std::min(point.y, min_y);
        max_x = std::max(point.x, max_x);
        max_y = std::max(point.y, max_y);
    }

    m_bbox.bl = glm::vec2(min_x, min_y);
    m_bbox.tr = glm::vec2(max_x, max_y);
}

void Collider::render_outline(Renderer *renderer, Color color)
{
    renderer->quad_line(quad(), 1.0f, color);
}

}  // namespace ITD

#pragma once
#include <functional>
#include "../maths/shapes.h"
#include "ecs.h"

namespace ITD {

struct Mask {
    static constexpr uint32_t None = 0;
    static constexpr uint32_t Solid = 1;
    static constexpr uint32_t Player = 1 << 1;
    static constexpr uint32_t Enemy = 1 << 2;
};

class Collider : public Component
{
    friend class CollisionHandler;
    friend class Entity;

private:
    struct Projection {
        float start;
        float end;
    };

public:
    uint32_t mask;
    uint32_t collides_with;
    bool active;
    bool trigger_only;
    std::function<bool(Collider *collider, Collider *other,
                       const glm::vec2 &dir)>
        on_collide;

private:
    Rectf m_bounds;
    float m_rotation;
    bool m_dynamic;
    bool m_invalid_cache;

    Quadf m_quad;
    glm::vec2 m_axes[2];
    Rectf m_bbox;

    Recti m_bucket_box;
    bool m_in_bucket;
    std::list<Collider *>::iterator m_dyn_iter;

public:
    Collider(const Rectf &bounds, float rotation = 0.0f,
             bool dynamic = true);

    void set_bounds(const Rectf &bounds);
    Rectf get_bounds() const;

    void set_rotation(float rotation);
    void rotate(float amount);
    float get_rotation() const;

    void face_towards(const glm::vec2 &dir);

    void set_dynamic(bool dynamic);
    bool is_dynamic() const;

    Quadf quad();
    Rectf bbox();

    bool overlaps(Collider &other);
    glm::vec2 push_out(Collider &other);
    float distance(Collider &other);

    Collider *check(uint32_t mask);
    void check_all(uint32_t mask, std::vector<Collider *> *out);

    void render_outline(Renderer *renderer, Color color);

protected:
    void awake() override;
    void on_removed() override;

private:
    // Assumes that the collider is refreshed
    Projection project(const glm::vec2 &axis) const;

    void on_position_changed();
    void refresh();
    void recalculate();
};

}  // namespace ITD

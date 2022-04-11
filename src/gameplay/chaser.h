#pragma once
#include "ecs.h"

namespace ITD {

class Chaser : public Component
{
private:
    static constexpr float max_speed = 150.0f;
    static constexpr float accel = 110.0f;
    static constexpr float hurt_knockback = 30.0f;
    static constexpr float rotation_multiplier = 0.5f;
    static constexpr float explosion_duration = 0.2f;
    static constexpr float explosion_width = 12.0f;
    static constexpr float explosion_height = 12.0f;

public:
    bool on_collide(Collider *other, const glm::vec2 &dir);
    void explode();

    void update(float elapsed) override;
    void render(Renderer *renderer) override;

    static Entity *create(Scene *scene, const glm::vec2 &pos);
};

}  // namespace ITD

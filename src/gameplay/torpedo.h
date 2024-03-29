#pragma once
#include "ecs.h"

namespace ITD {

class Torpedo : public Component
{
private:
    static constexpr float life_time = 1.5f;
    static constexpr float collider_width = 8.0f;
    static constexpr float collider_height = 4.0f;
    static constexpr float tracker_width = 144.0f;
    static constexpr float tracker_height = 64.0f;
    static constexpr float rotation_multiplier = 0.1f;
    static constexpr float accel = 200.0f;
    static constexpr float explosion_duration = 0.15f;
    static constexpr float explosion_width = 10.0f;
    static constexpr float explosion_height = 10.0f;
    static constexpr float max_speed = 220.0f;

    float m_life_timer;
    uint32_t m_target_id;

public:
    Entity *tracker;

public:
    Torpedo();

    void update(float elapsed) override;
    void render(Renderer *renderer) override;

    static Entity *create(Scene *scene, const glm::vec2 &pos,
                          const glm::vec2 &dir, const float start_speed);

private:
    void explode();
};

}  // namespace ITD

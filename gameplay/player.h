#pragma once
#include "ecs.h"

namespace Uboat
{
    class Player : public Component
    {
    private:
        static constexpr float max_speed = 100.0f;
        static constexpr float accel = 150.0f;
        static constexpr float dash_speed = 300.0f;
        static constexpr float dash_time = 0.01f;
        static constexpr float dash_deaccel = 600.0f;
        static constexpr float dash_cooldown = 1.0f;
        static constexpr float shoot_cooldown = 1.0f;
        static constexpr float shoot_speed = 250.0f;
        static constexpr float shoot_knockback = 35.0f;

        glm::vec2 m_facing;
        float m_dash_timer;
        float m_dash_cooldown_timer;
        float m_shoot_cooldown_timer;

    public:
        Player();

        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2& pos);
    };
}

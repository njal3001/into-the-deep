#pragma once
#include "ecs.h"

namespace ITD
{
    class Player;

    class Chaser : public Component
    {
    private:
        static constexpr float max_speed = 150.0f;
        static constexpr float accel = 110.0f;
        static constexpr float hurt_knockback = 30.0f;
        static constexpr float rotation_multiplier = 0.5f;
        static constexpr float detach_recovery_time = 1.0f;
        static constexpr float attack_time = 0.5f;

        Player *m_attached_to;
        float m_detach_recovery_timer;
        float m_attack_timer;

    public:
        Chaser();

        bool on_collide(Collider *other, const glm::vec2 &dir);
        void detach();

        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2& pos);
    };
}

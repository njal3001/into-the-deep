#pragma once
#include "ecs.h"

namespace Uboat
{
    class Rocket : public Component
    {
    private:
        static constexpr float life_time = 1.0f;
        static constexpr float collider_width = 8.0f;
        static constexpr float collider_height = 4.0f;
        static constexpr float tracker_width = 72.0f;
        static constexpr float tracker_height = 32.0f;
        static constexpr float rotation_multiplier = 0.2f;
        static constexpr float accel = 80.0f;

        float m_life_timer;
        float m_max_speed;
        Entity *m_target;

    public:
        Entity *tracker;

    public:
        Rocket();

        void awake() override;
        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2 &pos, const glm::vec2& vel);

    private:
        void on_hit(Collider *other, const glm::vec2 &dir);
    };
}

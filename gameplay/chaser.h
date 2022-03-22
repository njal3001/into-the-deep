#pragma once
#include "ecs.h"

namespace ITD
{
    class Chaser : public Component
    {
    private:
        static constexpr float max_speed = 150.0f;
        static constexpr float accel = 110.0f;
        static constexpr float hurt_knockback = 30.0f;
        static constexpr float rotation_multiplier = 0.5f;

    public:
        Chaser();

        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2& pos);
    };
}

#pragma once
#include "ecs.h"

namespace Uboat
{
    class Chaser : public Component
    {
    private:
        static constexpr float max_speed = 80.0f;
        static constexpr float accel = 100.0f;
        static constexpr float hurt_knockback = 10.0f;

    public:
        Chaser();

        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2& pos);
    };
}

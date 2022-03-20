#pragma once
#include "ecs.h"
#include <functional>

namespace Uboat
{
    class Hurtable : public Component
    {
    private:
        float m_invincible_timer;

    public:
        int health;
        float invincible_time;
        std::function<void (Hurtable *self, const glm::vec2 &dir)> on_hurt;
        
    public:
        Hurtable();

        void hurt(const glm::vec2 &dir);
        void update(const float elapsed) override;
    };
}

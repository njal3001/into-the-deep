#pragma once
#include "ecs.h"
#include <functional>

namespace Uboat
{
    class Hurtable : public Component
    {
    public:
        int health;
        std::function<void (Hurtable *self, const glm::vec2 &dir)> on_hurt;
        
        void hurt(const glm::vec2 &dir);
    };
}

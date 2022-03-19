#pragma once
#include "ecs.h"
#include <functional>
#include "collider.h"

namespace Uboat
{
    class Mover : public Component
    {
    private:
        static constexpr float collision_elasticity = 0.01f;

    public:
        Collider* collider;
        glm::vec2 vel;
        uint32_t collides_with;
        std::function<bool (Mover *mover, Collider *other, const glm::vec2 &dir)> on_hit;

    public:
        Mover();

        void update(const float elapsed) override;
    };
}

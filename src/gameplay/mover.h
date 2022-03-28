#pragma once
#include "ecs.h"
#include <functional>
#include "collider.h"

namespace ITD
{
    class Mover : public Component
    {
    public:
        glm::vec2 vel;
        //Collider* collider;
        //uint32_t collides_with;
        //std::function<bool (Mover *mover, Collider *other, const glm::vec2 &dir)> on_hit;

    public:
        Mover();

        void update(const float elapsed) override;
    };
}

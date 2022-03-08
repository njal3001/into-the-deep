#pragma once
#include "ecs.h"

namespace Uboat
{
    class Player : public Component
    {
    public:
        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2& pos);
    };


    template<>
    struct Properties<Player>
    {
        static uint8_t get()
        {
            return Property::Updatable | Property::Renderable;
        }
    };
}

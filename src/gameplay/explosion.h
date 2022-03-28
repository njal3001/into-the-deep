#pragma once
#include "ecs.h"

namespace ITD
{
    class Explosion : public Component
    {
    private:
        float m_duration_timer;

    public:
        Explosion(const float duration);

        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2 &pos, const float duration, const glm::vec2 &size, const float rotation);

    private:
        void on_hit(Collider *other, const glm::vec2 &dir);
    };
}

#pragma once
#include "ecs.h"

namespace ITD {

class Explosion : public Component
{
private:
    float m_duration_timer;

public:
    Explosion(float duration);

    void awake() override;

    void update(float elapsed) override;
    void render(Renderer *renderer) override;

    static Entity *create(Scene *scene, const glm::vec2 &pos, float duration,
                          const glm::vec2 &size, float rotation,
                          uint32_t hurt_mask);

private:
    void on_hit(Collider *other, const glm::vec2 &dir);
};

}  // namespace ITD

#pragma once
#include <functional>
#include "ecs.h"

namespace ITD {

class Hurtable : public Component
{
private:
    float m_invincible_timer;

public:
    int health;
    float invincible_time;
    std::function<void(Hurtable *self, const glm::vec2 &force)> on_hurt;

public:
    Hurtable();

    bool hurt(const glm::vec2 &dir);
    void update(float elapsed) override;
};

}  // namespace ITD

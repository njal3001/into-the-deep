#pragma once
#include "ecs.h"

namespace ITD {

class Mover : public Component
{
public:
    glm::vec2 vel;
    glm::vec2 facing;
    float target_speed;
    float accel;
    bool approach_target;

public:
    Mover();

    void rotate_towards(const glm::vec2 &target_dir, float amount);

    void update(float elapsed) override;
};

}  // namespace ITD

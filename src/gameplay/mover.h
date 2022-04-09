#pragma once
#include "ecs.h"

namespace ITD {

class Mover : public Component
{
public:
    glm::vec2 vel;

public:
    Mover();

    void update(const float elapsed) override;
};

}  // namespace ITD

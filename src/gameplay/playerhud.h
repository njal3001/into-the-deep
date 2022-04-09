#pragma once
#include "ecs.h"

namespace ITD {

class PlayerHUD : public Component
{
private:
    static constexpr float width = 4.0f;
    static constexpr float height = 4.0f;
    static constexpr float offset_x = 5.0f;
    static constexpr float offset_y = 10.0f;
    static constexpr float margin = 10.0f;

public:
    void render(Renderer *renderer) override;

    static Entity *create(Scene *scene);
};

}  // namespace ITD

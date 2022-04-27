#pragma once
#include "ecs.h"

namespace ITD {

class Wall : public Component
{
public:
    struct Direction {
        static constexpr uint8_t North = 1;
        static constexpr uint8_t East = 1 << 1;
        static constexpr uint8_t West = 1 << 2;
        static constexpr uint8_t South = 1 << 3;
    };

private:
    uint8_t m_directions;

public:
    Wall(uint8_t directions);

    void render(Renderer *renderer) override;

    static Entity *create(Scene *scene, const glm::vec2 &pos, uint8_t directions);
};
}  // namespace ITD

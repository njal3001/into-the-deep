#pragma once
#include <glm/glm.hpp>
#include "../maths/shapes.h"

namespace ITD {

class Camera
{
public:
    static Rectf make_fit(const glm::vec2 &size, float aspect_ratio);

    static glm::vec2 screen_to_world_pos(const glm::vec2 &screen_size, const Rectf &world_bounds, const glm::vec2 &pos);
};

}  // namespace ITD

#pragma once
#include <glm/glm.hpp>
#include "../maths/shapes.h"

namespace ITD {

class Camera
{
public:
    static Rectf make_fit(const glm::vec2 &size, float aspect_ratio);

};

}  // namespace ITD

#include "camera.h"

namespace ITD {

Rectf Camera::make_fit(const glm::vec2 &size, float aspect_ratio)
{
    glm::vec2 fit_size = size;
    if (size.x / aspect_ratio > size.y)
    {
        fit_size.y = size.x / aspect_ratio;
    }
    else
    {
        fit_size.x = size.y * aspect_ratio;
    }

    glm::vec2 offset = glm::vec2((fit_size.x - size.x) / 2.0f,
                                 (fit_size.y - size.y) / 2.0f);

    return Rectf(-offset, fit_size - offset);
}

glm::vec2 Camera::screen_to_world_pos(const glm::vec2 &screen_size, const Rectf &world_bounds, const glm::vec2 &pos)
{
    glm::vec2 normalized_pos = pos / screen_size;

    return normalized_pos * world_bounds.size() + world_bounds.bl;
}

}  // namespace ITD

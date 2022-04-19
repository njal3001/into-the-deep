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

    printf("Size: (%f, %f)\n", size.x, size.y);
    printf("Fit Size: (%f, %f)\n", fit_size.x, fit_size.y);
    glm::vec2 offset = glm::vec2((fit_size.x - size.x) / 2.0f,
                                 (fit_size.y - size.y) / 2.0f);

    return Rectf(-offset, fit_size - offset);
}

}  // namespace ITD

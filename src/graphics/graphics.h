#pragma once
#include <glm/glm.hpp>
#include "color.h"

namespace ITD {

namespace Graphics {

    constexpr glm::vec2 pixel_screen_size = glm::vec2(640.0f, 360.0f);

    bool init();

    void clear(Color color);
    void update_viewport(const glm::ivec2 &window_size);

    void present();
    void shutdown();
}  // namespace Graphics

}  // namespace ITD

#pragma once
#include "color.h"

namespace ITD {

namespace Graphics {
    bool init();
    void clear(Color color);
    void present();
    void shutdown();
}  // namespace Graphics

}  // namespace ITD

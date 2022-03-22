#pragma once
#include "color.h"

namespace ITD
{
    namespace Graphics
    {
        bool init();
        void clear(const Color color);
        void present();
        void shutdown();
    }
}

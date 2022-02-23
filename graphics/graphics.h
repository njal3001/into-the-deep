#pragma once
#include "color.h"

namespace Uboat
{
    namespace Graphics
    {
        bool init();
        void clear(const Color color);
        void present();
        void shutdown();
    }
}

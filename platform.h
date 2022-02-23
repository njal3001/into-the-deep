#pragma once
#include <SDL2/SDL.h>

namespace Uboat
{
    namespace Platform
    {
        bool init();
        bool update();
        void present();
        void shutdown();

        SDL_Window* get_window();
        SDL_Joystick* get_joystick();
    }
}

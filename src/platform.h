#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace ITD
{
    namespace Platform
    {
        constexpr uint64_t ticks_per_sec = 100000;

        bool init();
        bool update();
        void present();
        void shutdown();

        SDL_Window* get_window();
        SDL_Joystick* get_joystick();

        std::string app_path();

        uint64_t ticks();
    }
}

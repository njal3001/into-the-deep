#pragma once
#include <SDL2/SDL.h>

namespace Uboat
{
    struct Keystate
    {
        bool down;
        bool pressed;
        bool released;
    };

    namespace Input
    {
        void init();
        void update();

        Keystate kb_state(const SDL_Scancode key);
    }
}

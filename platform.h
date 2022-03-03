#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace Uboat
{
    struct File
    {
        std::string path;
        unsigned int size;
        char* data;
    };

    namespace Platform
    {
        bool init();
        bool update();
        void present();
        void shutdown();

        SDL_Window* get_window();
        SDL_Joystick* get_joystick();

        std::string app_path();
        File read_file(const std::string& file_path);

        uint64_t ticks();
    }
}

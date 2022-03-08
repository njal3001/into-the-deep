#include "platform.h"
#include "graphics/graphics.h"
#include "input.h"

namespace Uboat
{
    namespace
    {
        SDL_Window *g_window = nullptr;
        SDL_Joystick *g_joystick = nullptr;
        uint64_t g_ticks = 0;
        uint8_t g_frame = 0;
    }

    bool Platform::init()
    {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

        g_window = SDL_CreateWindow(
            "Uboat Game!",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            960,
            540,
            SDL_WINDOW_OPENGL
        );

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        if (!Graphics::init())
        {
            return false;
        }

        Input::init();

        return true;
    }

    bool Platform::update()
    {
        bool cont = true;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                cont = false;
            }
            else if (event.type == SDL_JOYDEVICEADDED)
            {
                if (!g_joystick)
                {
                    int index = event.jdevice.which;
                    if (SDL_IsGameController(index))
                    {
                        g_joystick = SDL_JoystickOpen(index);
                    }
                }
            }
            else if (event.type == SDL_JOYDEVICEREMOVED)
            {
                if (g_joystick && SDL_JoystickInstanceID(g_joystick) == event.jdevice.which)
                {
                    SDL_JoystickClose(g_joystick);
                    g_joystick = nullptr;
                }
            }
        }

        Input::update();

        g_frame++;

        return cont;
    }

    void Platform::shutdown()
    {
        Graphics::shutdown();
        SDL_DestroyWindow(g_window);
        SDL_Quit();
    }

    SDL_Window* Platform::get_window()
    {
        return g_window;
    }

    SDL_Joystick* Platform::get_joystick()
    {
        return g_joystick;
    }

    std::string Platform::app_path()
    {
        const char *path = SDL_GetBasePath();
        return std::string(path);
    }

    File Platform::read_file(const std::string& file_path)
    {
        File file =
            {
                .path = file_path,
                .size = 0,
                .data = nullptr
            };

        SDL_RWops* rw = SDL_RWFromFile(file_path.c_str(), "rb");
        if (!rw)
        {
            return file;
        }

        Sint64 size = SDL_RWsize(rw);
        char* buffer = new char[size];

        Sint64 read = SDL_RWread(rw, buffer, sizeof(char), size);

        SDL_RWclose(rw);
        if (read != size)
        {
            delete[] buffer;
            return file;
        }

        file.size = (unsigned int)size;
        file.data = buffer;

        return file;
    }

    uint64_t Platform::ticks()
    {
        auto counter = SDL_GetPerformanceCounter();
        auto freq = (double)SDL_GetPerformanceFrequency();
        return (uint64_t)(counter * (ticks_per_sec / freq));
    }

    uint8_t Platform::frame()
    {
        return g_frame;
    }
}

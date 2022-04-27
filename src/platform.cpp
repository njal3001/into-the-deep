#include "platform.h"
#include <SDL2/SDL_mixer.h>
#include "debug.h"
#include "graphics/graphics.h"
#include "input.h"

namespace ITD {

namespace {
    SDL_Window *g_window = nullptr;
    SDL_Joystick *g_joystick = nullptr;
    uint64_t g_ticks = 0;
    const char *g_app_path;
    bool g_muted = false;
}  // namespace

bool Platform::init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER |
             SDL_INIT_AUDIO);

    g_window = SDL_CreateWindow("Into The Deep", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, 960, 540,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!g_window)
    {
        Log::error("Could not create window");
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        Log::error("Could not initialize audio");
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    if (!Graphics::init())
    {
        Log::error("Could not initialize graphics");
        return false;
    }

    Input::init();

    g_app_path = SDL_GetBasePath();

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
            if (g_joystick &&
                SDL_JoystickInstanceID(g_joystick) == event.jdevice.which)
            {
                SDL_JoystickClose(g_joystick);
                g_joystick = nullptr;
            }
        }
        else if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                Graphics::update_viewport(
                    glm::ivec2(event.window.data1, event.window.data2));
            }
        }
    }

    Input::update();

    return cont;
}

void Platform::shutdown()
{
    Graphics::shutdown();
    Mix_Quit();
    SDL_DestroyWindow(g_window);
    SDL_free((void *)g_app_path);
    SDL_Quit();
}

SDL_Window *Platform::get_window()
{
    return g_window;
}

SDL_Joystick *Platform::get_joystick()
{
    return g_joystick;
}

std::string Platform::app_path()
{
    return std::string(g_app_path);
}

uint64_t Platform::ticks()
{
    auto counter = SDL_GetPerformanceCounter();
    auto freq = (double)SDL_GetPerformanceFrequency();
    return (uint64_t)(counter * (ticks_per_sec / freq));
}

void Platform::toggle_fullscreen()
{
    bool is_fullscreen = SDL_GetWindowFlags(g_window) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(g_window, (!is_fullscreen) * SDL_WINDOW_FULLSCREEN);
}

glm::ivec2 Platform::screen_size()
{
    glm::ivec2 size;
    SDL_GetWindowSize(g_window, &size.x, &size.y);

    return size;
}

void Platform::toggle_mute()
{
    g_muted = !g_muted;
}

bool Platform::muted()
{
    return g_muted;
}

void Platform::show_cursor(bool val)
{
    SDL_ShowCursor(val ? SDL_ENABLE : SDL_DISABLE);
}

void Platform::set_cursor_pos(const glm::ivec2 &new_pos)
{
    SDL_WarpMouseInWindow(Platform::get_window(), new_pos.x, new_pos.y);
}

}  // namespace ITD

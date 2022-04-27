#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <string>

namespace ITD {

namespace Platform {

    constexpr uint64_t ticks_per_sec = 100000;

    bool init();
    bool update();
    void present();
    void shutdown();

    SDL_Window *get_window();
    SDL_Joystick *get_joystick();

    std::string app_path();

    uint64_t ticks();

    void toggle_fullscreen();
    glm::ivec2 screen_size();

    void toggle_mute();
    bool muted();

    // TODO: This should be in Input
    void show_cursor(bool val);
    void set_cursor_pos(const glm::ivec2 &new_pos);

}  // namespace Platform

}  // namespace ITD

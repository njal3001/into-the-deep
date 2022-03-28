#pragma once
#include <SDL2/SDL.h>
#include <glm/vec2.hpp>

namespace ITD
{
    #define KEYBOARD_KEY_MAX 282
    #define MOUSE_BUTTON_MAX 6
    #define CONTROLLER_BUTTON_MAX 16
    #define CONTROLLER_AXES_MAX 7

    struct Keystate
    {
        bool down;
        bool pressed;
        bool released;
    };

    namespace Input
    {
        struct Keyboard
        {
            bool pressed[KEYBOARD_KEY_MAX];
            bool down[KEYBOARD_KEY_MAX];
            bool released[KEYBOARD_KEY_MAX];

            void clear();
            void update();
        };

        struct Mouse
        {
            bool pressed[MOUSE_BUTTON_MAX];
            bool down[MOUSE_BUTTON_MAX];
            bool released[MOUSE_BUTTON_MAX];

            glm::ivec2 pos;

            void clear();
            void update();
        };

        struct Controller
        {
            bool pressed[CONTROLLER_BUTTON_MAX];
            bool down[CONTROLLER_BUTTON_MAX];
            bool released[CONTROLLER_BUTTON_MAX];

            float axes[CONTROLLER_AXES_MAX];

            bool active() const;
            void clear();
            void update();
        };

        void init();
        void update();

        const Keyboard* keyboard();
        const Mouse* mouse();
        const Controller* controller();
    }
}

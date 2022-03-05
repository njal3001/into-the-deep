#include "input.h"
#include "platform.h"
#include <SDL2/SDL_joystick.h>

namespace Uboat
{
    void Input::Keyboard::clear()
    {
        for (size_t k = 0; k < KEYBOARD_KEY_MAX; k++)
        {
            down[k] = false;
            pressed[k] = false;
            released[k] = false;
        }
    }

    void Input::Keyboard::update()
    {
        const uint8_t *kb_state = SDL_GetKeyboardState(NULL);
        for (size_t k = 0; k < KEYBOARD_KEY_MAX; k++)
        {
            pressed[k] = !down[k] && kb_state[k];
            released[k] = down[k] && !kb_state[k];
            down[k] = kb_state[k];
        }
    }

    void Input::Mouse::clear()
    {
        for (size_t b = 0; b < MOUSE_BUTTON_MAX; b++)
        {
            down[b] = false;
            pressed[b] = false;
            released[b] = false;
        }
    }

    void Input::Mouse::update()
    {
        int x, y;
        const uint32_t bmask = SDL_GetMouseState(&x, &y);

        // Update buttons
        for (uint8_t b = 0; b < MOUSE_BUTTON_MAX; b++)
        {
            bool bstate = SDL_BUTTON(b) & bmask;

            pressed[b] = !down[b] && bstate;
            released[b] = down[b] && !bstate;
            down[b] = bstate;
        }

        // Update position
        pos.x = x;
        pos.y = y;
    }

    bool Input::Controller::active() const
    {
        return Platform::get_joystick();
    }

    void Input::Controller::clear()
    {
        for (size_t b = 0; b < CONTROLLER_BUTTON_MAX; b++)
        {
            down[b] = false;
            pressed[b] = false;
            released[b] = false;
        }

        for (size_t a = 0; a < CONTROLLER_AXES_MAX; a++)
        {
            axes[a] = 0;
        }
    }

    void Input::Controller::update()
    {
        SDL_Joystick *js = Platform::get_joystick();
        if (js)
        {
            // Update buttons
            for (int b = 0; b < CONTROLLER_BUTTON_MAX; b++)
            {
                bool bstate = SDL_JoystickGetButton(js, b);

                pressed[b] = !down[b] && bstate;
                released[b] = down[b] && !bstate;
                down[b] = bstate;
            }

            // Update axes
            for (int a = 0; a < CONTROLLER_AXES_MAX; a++)
            {
                float val = SDL_JoystickGetAxis(js, a);

                // Normalize
                if (val >= 0)
                {
                    val /= 32767.0f;
                }
                else
                {
                    val /= 32768.0f;
                }

                axes[a] = val;
            }
        }
        else
        {
            clear();
        }
    }

    namespace
    {
        Input::Keyboard g_keyboard;
        Input::Mouse g_mouse;
        Input::Controller g_controller;
    }

    void Input::init()
    {
        g_keyboard.clear();
        g_mouse.clear();
        g_controller.clear();
    }

    void Input::update()
    {
        g_keyboard.update();
        g_mouse.update();
        g_controller.update();
    }

    const Input::Keyboard* Input::keyboard()
    {
        return &g_keyboard;
    }

    const Input::Mouse* Input::mouse()
    {
        return &g_mouse;
    }

    const Input::Controller* Input::controller()
    {
        return &g_controller;
    }
}

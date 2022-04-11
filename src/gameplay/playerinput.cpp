#include "playerinput.h"
#include <SDL2/SDL.h>
#include "../input.h"
#include "../maths/calc.h"

namespace ITD {

PlayerInput::PlayerInput()
    : m_shoot_buffer_timer(0.0f)
    , m_dash_buffer_timer(0.0f)
{
}

void PlayerInput::update(float elapsed)
{
    m_dir.x = Input::keyboard()->down[SDL_SCANCODE_RIGHT] -
            Input::keyboard()->down[SDL_SCANCODE_LEFT];

    m_dir.y = Input::keyboard()->down[SDL_SCANCODE_UP] -
            Input::keyboard()->down[SDL_SCANCODE_DOWN];

    const Input::Controller *controller = Input::controller();
    if (controller->active() && m_dir == glm::vec2())
    {
        m_dir.x = controller->axes[0];
        m_dir.y = -controller->axes[1];
    }

    m_dir = Calc::normalize(m_dir);

    m_dash_buffer_timer = std::max(0.0f, m_dash_buffer_timer - elapsed);
    m_shoot_buffer_timer = std::max(0.0f, m_shoot_buffer_timer - elapsed);

    bool shoot_input = Input::keyboard()->pressed[SDL_SCANCODE_C];
    if (controller->active())
    {
        shoot_input |= controller->pressed[3];
    }

    if (shoot_input)
    {
        m_shoot_buffer_timer = input_buffer_time;
    }

    bool dash_input = Input::keyboard()->pressed[SDL_SCANCODE_X];
    if (controller->active())
    {
        dash_input |= controller->pressed[0];
    }

    if (dash_input)
    {
        m_dash_buffer_timer = input_buffer_time;
    }
}

glm::vec2 PlayerInput::dir() const
{
    return m_dir;
}

bool PlayerInput::shoot() const
{
    return m_shoot_buffer_timer > 0.0f;
}

bool PlayerInput::dash() const
{
    return m_dash_buffer_timer > 0.0f;
}

bool PlayerInput::consume_dash()
{
    bool val = m_dash_buffer_timer > 0.0f;
    m_dash_buffer_timer = 0.0f;
    return val;
}

bool PlayerInput::consume_shoot()
{
    bool val = m_shoot_buffer_timer > 0.0f;
    m_shoot_buffer_timer = 0.0f;
    return val;
}

}  // namespace ITD

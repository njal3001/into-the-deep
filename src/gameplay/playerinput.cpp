#include "playerinput.h"
#include <SDL2/SDL.h>
#include "../input.h"
#include "../maths/calc.h"
#include "../platform.h"
#include "camera.h"
#include "collider.h"
#include "player.h"

namespace ITD {

PlayerInput::PlayerInput()
    : m_shoot_buffer_timer(0.0f)
    , m_dash_buffer_timer(0.0f)
{
}

void PlayerInput::update(Player *player, float elapsed)
{
    const Input::Keyboard *keyboard = Input::keyboard();
    const Input::Controller *controller = Input::controller();

    glm::vec2 player_pos = player->entity()->get_pos();

    if (controller->active())
    {
        m_move_dir.x = controller->axes[0];
        m_move_dir.y = -controller->axes[1];
    }
    else
    {
        m_move_dir.x = Input::keyboard()->down[SDL_SCANCODE_D] -
                       Input::keyboard()->down[SDL_SCANCODE_A];

        m_move_dir.y = Input::keyboard()->down[SDL_SCANCODE_W] -
                       Input::keyboard()->down[SDL_SCANCODE_S];
    }

    m_move_dir = Calc::normalize(m_move_dir);

    m_dash_buffer_timer = std::max(0.0f, m_dash_buffer_timer - elapsed);
    m_shoot_buffer_timer = std::max(0.0f, m_shoot_buffer_timer - elapsed);

    bool shoot_input = keyboard->pressed[SDL_SCANCODE_X];
    if (controller->active())
    {
        shoot_input |= controller->pressed[3];
    }

    if (shoot_input)
    {
        m_shoot_buffer_timer = input_buffer_time;
    }

    bool dash_input = Input::keyboard()->pressed[SDL_SCANCODE_C];
    if (controller->active())
    {
        dash_input |= controller->pressed[0];
    }

    if (dash_input)
    {
        m_dash_buffer_timer = input_buffer_time;
    }
}

glm::vec2 PlayerInput::move_dir() const
{
    return m_move_dir;
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

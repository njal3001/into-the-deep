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
    const Input::Controller *controller = Input::controller();
    const Input::Mouse *mouse = Input::mouse();

    if (controller->active())
    {
        m_move_dir.x = controller->axes[0];
        m_move_dir.y = -controller->axes[1];

        m_shoot_dir.x = controller->axes[2];
        m_shoot_dir.y = -controller->axes[3];
    }
    else
    {
        m_move_dir.x = Input::keyboard()->down[SDL_SCANCODE_D] -
                       Input::keyboard()->down[SDL_SCANCODE_A];

        m_move_dir.y = Input::keyboard()->down[SDL_SCANCODE_W] -
                       Input::keyboard()->down[SDL_SCANCODE_S];

        Collider *player_collider = player->get<Collider>();
        if (player_collider)
        {
            Quadf quad = player_collider->quad();
            glm::vec2 center = quad.center();

            glm::vec2 screen_size = glm::vec2(Platform::screen_size());

            glm::vec2 target_pos = Camera::screen_to_world_pos(
                screen_size,
                player->scene()->world_bounds(),
                glm::vec2(mouse->pos.x, screen_size.y - mouse->pos.y));
            m_shoot_dir = target_pos - center;
        }
        else
        {
            m_shoot_dir = m_move_dir;
        }
    }

    m_move_dir = Calc::normalize(m_move_dir);
    m_shoot_dir = Calc::normalize(m_shoot_dir);

    m_dash_buffer_timer = std::max(0.0f, m_dash_buffer_timer - elapsed);
    m_shoot_buffer_timer = std::max(0.0f, m_shoot_buffer_timer - elapsed);

    bool shoot_input = mouse->pressed[1];
    if (controller->active())
    {
        shoot_input |= controller->pressed[3];
    }

    if (shoot_input)
    {
        m_shoot_buffer_timer = input_buffer_time;
    }

    bool dash_input = Input::keyboard()->pressed[SDL_SCANCODE_SPACE];
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

glm::vec2 PlayerInput::shoot_dir() const
{
    return m_shoot_dir;
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

#include "player.h"
#include "collider.h"
#include "mover.h"
#include "../input.h"
#include "../maths/calc.h"
#include <glm/gtx/vector_angle.hpp>

namespace Uboat
{
    Player::Player()
        : m_dash_dir(glm::vec2(0.0f, 1.0f)), m_dash_timer(0.0f), m_dash_cooldown_timer(0.0f)
    {}

    void Player::update(const float elapsed)
    {
        glm::vec2 dir;

        const Input::Controller *controller = Input::controller();

        if (controller->active())
        {
            dir.x = controller->axes[0];
            dir.y = -controller->axes[1];
        }
        else
        {
            dir.x = Input::keyboard()->down[SDL_SCANCODE_RIGHT]
                - Input::keyboard()->down[SDL_SCANCODE_LEFT];

            dir.y = Input::keyboard()->down[SDL_SCANCODE_UP]
                - Input::keyboard()->down[SDL_SCANCODE_DOWN];
        }

        dir = Calc::normalize(dir);

        auto col = get<Collider>();
        if (dir.x != 0 || dir.y != 0)
        {
            col->rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), glm::vec2(1.0f, 0.0f));
            m_dash_dir = dir;
        }

        auto mover = get<Mover>();

        if (m_dash_timer > 0.0f)
        {
            m_dash_timer -= elapsed;
        }
        else
        {
            if (glm::length2(mover->vel) > max_speed * max_speed)
            {
                mover->vel = Calc::approach(mover->vel, dir * max_speed, dash_deaccel * elapsed);
            }
            else
            {
                mover->vel = Calc::approach(mover->vel, dir * max_speed, accel * elapsed);
            }
        }

        // Check for dash input
        if (m_dash_cooldown_timer <= 0.0f)
        {
            bool dash_started = false;
            if (controller->active())
            {
                dash_started = controller->pressed[0];
            }
            else
            {
                dash_started = Input::keyboard()->pressed[SDL_SCANCODE_X];
            }

            if (dash_started)
            {
                mover->vel = m_dash_dir * dash_speed;
                m_dash_timer = dash_time;
                m_dash_cooldown_timer = dash_cooldown;
            }
        }
        else if (m_dash_timer <= 0.0f)
        {
            m_dash_cooldown_timer -= elapsed;
        }
    }

    void Player::render(Renderer *renderer)
    {
        Collider *col = get<Collider>();

        // const Rectf& bbox = col->bbox();
        // renderer->rect(bbox.bl, bbox.tr, Color::white);

        const Quadf& quad = col->quad();
        renderer->quad(quad.a, quad.b, quad.c, quad.d, Color::black);
    }

    Entity* Player::create(Scene *scene, const glm::vec2& pos)
    {
        Entity *e = scene->add_entity(pos);
        e->add(new Player());

        Collider *c = new Collider(Rectf(glm::vec2(0.0f, 0.0f), glm::vec2(12.0f, 8.0f)));
        c->mask = Mask::Player;
        e->add(c);

        Mover *m = new Mover();
        m->stop_mask |= Mask::Enemy;
        m->collider = c;
        e->add(m);

        return e;
    }
}

#include "player.h"
#include "collider.h"
#include "mover.h"
#include "../input.h"
#include "../maths/calc.h"
#include <glm/gtx/vector_angle.hpp>
#include "bullet.h"

namespace Uboat
{
    Player::Player()
        : m_facing(glm::vec2(1.0f, 0.0f)), m_dash_timer(0.0f), m_dash_cooldown_timer(0.0f), 
        m_shoot_cooldown_timer(0.0f)
    {}

    void Player::update(const float elapsed)
    {
        glm::vec2 dir;

        dir.x = Input::keyboard()->down[SDL_SCANCODE_RIGHT]
            - Input::keyboard()->down[SDL_SCANCODE_LEFT];

        dir.y = Input::keyboard()->down[SDL_SCANCODE_UP]
            - Input::keyboard()->down[SDL_SCANCODE_DOWN];

        const Input::Controller *controller = Input::controller();
        if (controller->active() && dir == glm::vec2())
        {
            dir.x = controller->axes[0];
            dir.y = -controller->axes[1];
        }

        dir = Calc::normalize(dir);

        auto col = get<Collider>();
        if (dir.x != 0 || dir.y != 0)
        {
            col->rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), glm::vec2(1.0f, 0.0f));
            m_facing = dir;
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
            bool dash_started = Input::keyboard()->pressed[SDL_SCANCODE_X];
            if (controller->active())
            {
                dash_started |= controller->pressed[0];
            }

            if (dash_started)
            {
                mover->vel = m_facing * dash_speed;
                m_dash_timer = dash_time;
                m_dash_cooldown_timer = dash_cooldown;
            }
        }
        else if (m_dash_timer <= 0.0f)
        {
            m_dash_cooldown_timer -= elapsed;
        }

        // Check for shooting input
        if (m_dash_timer <= 0.0f && m_shoot_cooldown_timer <= 0.0f)
        {
            bool shoot = Input::keyboard()->pressed[SDL_SCANCODE_C];
            if (controller->active())
            {
                shoot |= controller->pressed[3];
            }

            if (shoot)
            {
                Bullet::create(scene(), m_entity->pos, m_facing * shoot_speed);
                m_shoot_cooldown_timer = shoot_cooldown;

                mover->vel -= m_facing * shoot_knockback;
            }
        }
        else
        {
            m_shoot_cooldown_timer -= elapsed;
        }
    }

    void Player::render(Renderer *renderer)
    {
        Collider *col = get<Collider>();

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
        m->collides_with |= Mask::Enemy;
        m->collider = c;
        e->add(m);

        return e;
    }
}

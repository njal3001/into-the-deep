#include "player.h"
#include "collider.h"
#include "mover.h"
#include "../input.h"
#include "../maths/calc.h"
#include <glm/gtx/vector_angle.hpp>
#include "rocket.h"
#include "hurtable.h"

namespace ITD
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
        float moving = 1.0f;

        auto col = get<Collider>();
        if (dir.x != 0 || dir.y != 0)
        {
            const glm::vec2 right = glm::vec2(1.0f, 0.0f);
            const float target_rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), right);
            col->rotation = Calc::shortest_rotation_approach(col->rotation, target_rotation, rotation_multiplier * elapsed);
            m_facing = glm::rotate(right, col->rotation);
        }
        else
        {
            moving = 0.0f;
        }

        auto mover = get<Mover>();

        if (m_dash_timer > 0.0f)
        {
            m_dash_timer -= elapsed;
        }
        else
        {
            const float a = glm::length2(mover->vel) > max_speed * max_speed ? dash_deaccel : accel;
            mover->vel = Calc::approach(mover->vel, m_facing * moving * max_speed, a * elapsed);
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
                Rocket::create(scene(), m_entity->pos, m_facing * shoot_speed);
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

        Hurtable *h = new Hurtable();
        h->health = 5;
        e->add(h);

        return e;
    }
}

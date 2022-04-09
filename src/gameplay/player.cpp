#include "player.h"
#include <glm/gtx/vector_angle.hpp>
#include "../input.h"
#include "../maths/calc.h"
#include "animator.h"
#include "collider.h"
#include "hurtable.h"
#include "mover.h"
#include "rocket.h"

namespace ITD {

Player::Player()
    : m_facing(glm::vec2(1.0f, 0.0f))
    , m_dash_timer(0.0f)
    , m_dash_cooldown_timer(0.0f)
    , m_shoot_cooldown_timer(0.0f)
{
}

void Player::update(const float elapsed)
{
    glm::vec2 dir;

    dir.x = Input::keyboard()->down[SDL_SCANCODE_RIGHT] -
            Input::keyboard()->down[SDL_SCANCODE_LEFT];

    dir.y = Input::keyboard()->down[SDL_SCANCODE_UP] -
            Input::keyboard()->down[SDL_SCANCODE_DOWN];

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
        const float target_rotation =
            glm::orientedAngle(glm::vec2(dir.x, -dir.y), right);
        col->set_rotation(Calc::shortest_rotation_approach(
            col->get_rotation(), target_rotation,
            rotation_multiplier * elapsed));
        m_facing = glm::rotate(right, col->get_rotation());
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
        const float a = glm::length2(mover->vel) > max_speed * max_speed
                            ? dash_deaccel
                            : accel;
        mover->vel = Calc::approach(mover->vel, m_facing * moving * max_speed,
                                    a * elapsed);
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
            Rocket::create(scene(), m_entity->get_pos(),
                           m_facing * shoot_speed);
            m_shoot_cooldown_timer = shoot_cooldown;

            mover->vel -= m_facing * shoot_knockback;
        }
    }
    else
    {
        m_shoot_cooldown_timer -= elapsed;
    }

    auto anim = get<Animator>();
    anim->rotation = col->get_rotation();
}

void Player::render(Renderer *renderer)
{
    Collider *col = get<Collider>();
    Quadf quad = col->quad();
    // renderer->quad(quad.a, quad.b, quad.c, quad.d, Color::red);
}

Entity *Player::create(Scene *scene, const glm::vec2 &pos)
{
    Entity *ent = scene->add_entity(pos);
    ent->add(new Player());

    Collider *col =
        new Collider(Rectf(glm::vec2(0.0f, 0.0f), glm::vec2(12.0f, 7.0f)));
    col->mask = Mask::Player;
    col->collides_with = Mask::Solid | Mask::Enemy;
    ent->add(col);

    Mover *mov = new Mover();
    ent->add(mov);

    Hurtable *hur = new Hurtable();
    hur->health = 5;
    ent->add(hur);

    Animator *ani =
        new Animator("player", Recti(glm::ivec2(0, 0), glm::ivec2(15, 9)), 1,
                     0.0f, glm::vec2(-2.0f, -1.0f), glm::vec2(6.0f, 3.5f));
    ent->add(ani);

    return ent;
}

}  // namespace ITD

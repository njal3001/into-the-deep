#include "player.h"
#include <glm/gtx/vector_angle.hpp>
#include "../input.h"
#include "../maths/calc.h"
#include "../platform.h"
#include "../sound.h"
#include "animator.h"
#include "collider.h"
#include "content.h"
#include "hurtable.h"
#include "mover.h"
#include "torpedo.h"

namespace ITD {

Player::Player()
    : m_dash_timer(0.0f)
    , m_dash_cooldown_timer(0.0f)
    , m_shoot_cooldown_timer(0.0f)
    , m_torpedo_ammo(max_torpedo_ammo)
    , m_reload_timer(0.0f)
    , m_shoot_delay_timer(0.0f)
{
}

void Player::update(float elapsed)
{
    m_player_input.update(this, elapsed);
    glm::vec2 move_dir = m_player_input.move_dir();

    float moving = 1.0f;

    auto mover = get<Mover>();

    auto col = get<Collider>();
    if (move_dir.x != 0 || move_dir.y != 0)
    {
        mover->rotate_towards(move_dir,
                              Calc::TAU * rotation_multiplier * elapsed);
    }
    else
    {
        moving = 0.0f;
    }

    if (m_dash_timer > 0.0f)
    {
        m_dash_timer -= elapsed;
    }
    else
    {
        mover->approach_target = true;
        mover->accel = glm::length2(mover->vel) > max_speed * max_speed
                           ? dash_deaccel
                           : accel;

        mover->target_speed = moving * max_speed;
    }

    // Dashing
    if (m_dash_cooldown_timer <= 0.0f)
    {
        if (m_player_input.consume_dash())
        {
            mover->facing = move_dir;
            mover->vel = mover->facing * dash_speed;
            mover->approach_target = false;
            m_dash_timer = dash_time;
            m_dash_cooldown_timer = dash_cooldown;

            Sound *sfx = Content::find_sound("dash");
            sfx->play();
        }
    }
    else if (m_dash_timer <= 0.0f)
    {
        m_dash_cooldown_timer -= elapsed;
    }

    // Reloading
    if (m_torpedo_ammo == 0)
    {
        m_reload_timer += elapsed;
        if (m_reload_timer >= reload_time)
        {
            m_reload_timer = 0.0f;
            m_torpedo_ammo = max_torpedo_ammo;
        }
    }

    // Shooting
    if (m_dash_timer <= 0.0f && m_shoot_delay_timer <= 0.0f &&
        m_torpedo_ammo > 0)
    {
        if (m_player_input.consume_shoot())
        {
            auto vel_norm = Calc::normalize(mover->vel);
            float cos = glm::dot(vel_norm, mover->facing);

            glm::vec2 shoot_dir = mover->facing;

            Torpedo::create(scene(), m_entity->get_pos(), shoot_dir,
                            std::max(0.0f, cos) * mover->vel.length() +
                                torpedo_start_speed);

            m_shoot_delay_timer = shoot_delay;
            m_torpedo_ammo--;

            mover->vel -= shoot_dir * shoot_knockback;

            Sound *shoot_sfx = Content::find_sound("shoot");
            shoot_sfx->play();
        }
    }
    else
    {
        m_shoot_delay_timer -= elapsed;
    }

    col->face_towards(mover->facing);

    auto anim = get<Animator>();
    anim->rotation = col->get_rotation();
}

int Player::torpedo_ammo() const
{
    return m_torpedo_ammo;
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
    hur->on_hurt = [](Hurtable *self, const glm::vec2 &force) {
        Sound *sfx = Content::find_sound("hurt");
        sfx->play();
    };

    ent->add(hur);

    Animator *ani =
        new Animator("player", Recti(glm::ivec2(0, 0), glm::ivec2(15, 9)), 1,
                     0.0f, glm::vec2(-2.0f, -1.0f), glm::vec2(6.0f, 3.5f));
    ent->add(ani);

    return ent;
}

}  // namespace ITD

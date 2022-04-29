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
    , m_wing_rotation(0.0f)
{
}

void Player::update(float elapsed)
{
    m_player_input.update(this, elapsed);
    glm::vec2 move_dir = m_player_input.move_dir();

    float moving = 1.0f;

    auto mover = get<Mover>();
    auto hurtable = get<Hurtable>();

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

        if (m_dash_timer <= 0.0f)
        {
            hurtable->invincible = false;
        }
    }
    
    if (m_dash_timer - dash_time + dash_max_speed_time <= 0.0f)
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
            if (move_dir != glm::vec2())
            {
                mover->facing = move_dir;
            }

            mover->vel = mover->facing * dash_speed;
            mover->approach_target = false;
            m_dash_timer = dash_time;
            m_dash_cooldown_timer = dash_cooldown;

            hurtable->invincible = true;

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
            float vel_facing_cos = glm::dot(vel_norm, mover->facing);

            glm::vec2 shoot_dir = mover->facing;

            Torpedo::create(
                scene(), m_entity->get_pos(), shoot_dir,
                std::max(0.0f, vel_facing_cos) * glm::length(mover->vel) +
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

    // Update wing rotation
    float movedir_facing_cos = glm::dot(mover->facing, move_dir);
    float wing_rotation_delta = std::max(movedir_facing_cos, 0.0f) * 30.0f * elapsed;
    m_wing_rotation = fmod(m_wing_rotation + wing_rotation_delta, Calc::TAU);

    // TODO: Implement dash shield
}

void Player::render(Renderer *renderer)
{
    Collider *collider = get<Collider>();
    Quadf quad = collider->quad();
    glm::vec2 quad_center = quad.center();

    float wing_span = 5.0f;
    Trif right_wing = create_wing(quad.a, quad.d, -wing_span);
    Trif left_wing = create_wing(quad.b, quad.c, wing_span);

    float all_scale = 1.25f;
    glm::mat4 all_scale_matrix = Calc::scale(glm::vec2(all_scale, all_scale), quad_center);

    renderer->push_matrix(all_scale_matrix);
    renderer->quad_line(quad, 1.0f, Color::yellow);
    renderer->pop_matrix();

    renderer->push_matrix(Calc::rotate(m_wing_rotation, quad_center, glm::vec3(quad.d - quad.a, 0.0f))); 
    renderer->push_matrix(all_scale_matrix);

    if (m_dash_timer > 0.0f)
    {
        float wing_scale = 1.75f;
        renderer->push_matrix(Calc::scale(glm::vec2(wing_scale, wing_scale), right_wing.center()));
        renderer->tri(right_wing.a, right_wing.b, right_wing.c, Color::yellow);
        renderer->pop_matrix();

        renderer->push_matrix(Calc::scale(glm::vec2(wing_scale, wing_scale), left_wing.center()));
        renderer->tri(left_wing.a, left_wing.b, left_wing.c, Color::yellow);
        renderer->pop_matrix();
    }
    else
    {
        renderer->tri(right_wing.a, right_wing.b, right_wing.c, Color::yellow);
        renderer->tri(left_wing.a, left_wing.b, left_wing.c, Color::yellow);
    }

    renderer->pop_matrix();
    renderer->pop_matrix();
}

Trif Player::create_wing(const glm::vec2 &line_start, const glm::vec2 &line_end, float span) const
{
    Trif result;
    result.a = line_start;
    result.b = (line_start + line_end) / 2.0f;

    glm::vec2 line_dir = Calc::normalize(line_end - line_start);
    glm::vec line_normal = glm::vec2(-line_dir.y, line_dir.x);

    result.c = result.a + line_normal * span;

    return result;
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

    return ent;
}

}  // namespace ITD

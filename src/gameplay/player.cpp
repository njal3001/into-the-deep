#include "player.h"
#include <glm/gtx/vector_angle.hpp>
#include <random>
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
    , m_new_particle_timer(new_particle_time)
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

    float positive_accel_multiplier =
        std::max(glm::dot(mover->facing, move_dir), 0.0f);

    // Update wing rotation
    m_wing_rotation =
        fmod(m_wing_rotation + positive_accel_multiplier * 30.0f * elapsed,
             Calc::TAU);

    // Add particles
    m_new_particle_timer -= elapsed * positive_accel_multiplier;
    if (m_new_particle_timer <= 0.0f)
    {
        Quadf quad = col->quad();
        glm::vec2 pos = (quad.a + quad.b) / 2.0f;
        float speed = std::min(glm::length(mover->vel), 50.0f) * positive_accel_multiplier;

        ParticleSystem *psystem = scene()->particle_system();
        psystem->add_particle(pos, -Calc::normalize(mover->vel) * speed, 3.0f,
                              1.0f);
        m_new_particle_timer = new_particle_time;
    }

    // TODO: Implement dash shield
}

void Player::render(Renderer *renderer)
{
    Collider *collider = get<Collider>();
    Quadf quad = collider->quad();
    float quad_depth = 2.0f;
    glm::vec3 quad_center = glm::vec3(quad.center(), -quad_depth / 2.0f);

    float wing_span = 3.0f;
    Trif right_wing = create_wing(quad.a, quad.d, -wing_span);
    Trif left_wing = create_wing(quad.b, quad.c, wing_span);

    float all_scale = 1.25f;
    glm::mat4 all_scale_matrix =
        Calc::scale(glm::vec3(all_scale, all_scale, all_scale), quad_center);

    renderer->push_matrix(all_scale_matrix);
    renderer->quad_line(quad, 1.0f, Color::yellow);
    renderer->pop_matrix();

    // TODO: Hide wing that has rotated behind quad. Try depth testing?
    renderer->push_matrix(Calc::rotate(
        m_wing_rotation, glm::vec3(quad_center.x, quad_center.y, 0.0f),
        glm::vec3(quad.d - quad.a, 0.0f)));
    renderer->push_matrix(all_scale_matrix);

    render_wing(renderer, right_wing, 0.0f, quad_depth, Color::yellow);
    render_wing(renderer, left_wing, 0.0f, quad_depth, Color::yellow);

    renderer->pop_matrix();
    renderer->pop_matrix();
}

Trif Player::create_wing(const glm::vec2 &line_start, const glm::vec2 &line_end,
                         float span) const
{
    Trif result;
    result.a = line_start;
    result.b = (line_start + line_end) / 2.0f;

    glm::vec2 line_dir = Calc::normalize(line_end - line_start);
    glm::vec line_normal = glm::vec2(-line_dir.y, line_dir.x);

    result.c = result.a + line_normal * span;

    return result;
}

void Player::render_wing(Renderer *renderer, const Trif &wing, float z,
                         float depth, Color color) const
{
    glm::vec3 a_upper = glm::vec3(wing.a, z);
    glm::vec3 b_upper = glm::vec3(wing.b, z);
    glm::vec3 c_upper = glm::vec3(wing.c, z);

    glm::vec3 a_lower = glm::vec3(wing.a, z + depth);
    glm::vec3 b_lower = glm::vec3(wing.b, z + depth);
    glm::vec3 c_lower = glm::vec3(wing.c, z + depth);

    renderer->tri(a_upper, b_upper, c_upper, color);
    renderer->tri(a_lower, b_lower, c_lower, color);

    // Quads connecting upper and lower triangle
    renderer->quad(a_upper, a_lower, b_upper, b_lower, color);
    renderer->quad(b_upper, b_lower, c_upper, c_lower, color);
    renderer->quad(c_upper, c_lower, a_upper, a_lower, color);
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

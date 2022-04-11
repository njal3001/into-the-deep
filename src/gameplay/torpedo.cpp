#include "torpedo.h"
#include <glm/gtx/vector_angle.hpp>
#include "../maths/calc.h"
#include "animator.h"
#include "collider.h"
#include "explosion.h"
#include "hurtable.h"
#include "mover.h"
#include "tilemap.h"

namespace ITD {

Torpedo::Torpedo()
    : m_life_timer(life_time)
    , m_target(nullptr)
    , tracker(nullptr)
{
}

void Torpedo::awake()
{
    Collider *col = get<Collider>();
    col->set_rotation(
        glm::orientedAngle(glm::vec2(dir.x, -dir.y), glm::vec2(1.0f, 0.0f)));
}

void Torpedo::update(float elapsed)
{
    m_life_timer -= elapsed;
    if (m_life_timer <= 0.0f)
    {
        explode();
    }
    else
    {
        Collider *collider = get<Collider>();
        Collider *tracker_collider = tracker->get<Collider>();
        if (!m_target && tracker_collider)
        {
            glm::vec2 tracker_dir =
                glm::rotate(Calc::right, collider->get_rotation());

            tracker->set_pos(m_entity->get_pos() +
                             tracker_dir * (tracker_width - collider_width) /
                                 2.0f);

            tracker_collider->set_rotation(collider->get_rotation());

            std::vector<Collider *> in_range;
            tracker_collider->check_all(Mask::Enemy, &in_range);

            float min_dist = FLT_MAX;
            for (auto other : in_range)
            {
                float dist = collider->distance(*other);
                if (dist < min_dist)
                {
                    min_dist = dist;
                    m_target = other->entity();
                }
            }
        }

        Mover *mov = get<Mover>();
        glm::vec2 facing;
        if (m_target)
        {
            glm::vec2 dir =
                Calc::normalize(m_target->get_pos() - entity()->get_pos());

            float target_rotation =
                glm::orientedAngle(glm::vec2(dir.x, -dir.y), Calc::right);

            collider->set_rotation(Calc::shortest_rotation_approach(
                collider->get_rotation(), target_rotation,
                Calc::TAU * rotation_multiplier * elapsed));

            facing = glm::rotate(Calc::right, collider->get_rotation());
        }
        else
        {
            facing = dir;
        }

        mov->vel =
            Calc::approach(mov->vel, facing * max_speed, accel * elapsed);

        Animator *anim = get<Animator>();
        anim->rotation = collider->get_rotation();
    }
}

void Torpedo::explode()
{
    Collider *col = get<Collider>();
    Explosion::create(scene(), m_entity->get_pos() + col->get_bounds().center(),
                      explosion_duration,
                      glm::vec2(explosion_width, explosion_height),
                      col->get_rotation(), Mask::Enemy);
    tracker->destroy();
    m_entity->destroy();
}

Entity *Torpedo::create(Scene *scene, const glm::vec2 &pos,
                        const glm::vec2 &dir, const float start_speed)
{
    Entity *ent = scene->add_entity(pos);
    Torpedo *torpedo = new Torpedo();
    torpedo->dir = dir;
    ent->add(torpedo);

    Collider *col = new Collider(
        Rectf(glm::vec2(), glm::vec2(collider_width, collider_height)));
    col->collides_with = Mask::Solid | Mask::Enemy;
    col->trigger_only = true;
    col->on_collide = [](Collider *collider, Collider *other,
                         const glm::vec2 &dir) {
        Torpedo *torpedo = collider->get<Torpedo>();
        torpedo->explode();

        return true;
    };

    ent->add(col);

    Mover *mov = new Mover();
    mov->vel = dir * start_speed;
    ent->add(mov);

    Entity *tracker = scene->add_entity(pos);

    glm::vec2 tracker_bl = col->get_bounds().center() -
                           glm::vec2(tracker_width, tracker_height) / 2.0f;

    glm::vec2 tracker_tr =
        tracker_bl + glm::vec2(tracker_width, tracker_height);

    Collider *tracker_collider = new Collider(Rectf(tracker_bl, tracker_tr));

    tracker->add(tracker_collider);
    torpedo->tracker = tracker;

    Animator *anim =
        new Animator("torpedo", Recti(glm::ivec2(), glm::ivec2(8, 4)), 1, 0.0f,
                     glm::vec2(), glm::vec2(4.0f, 2.0f));
    ent->add(anim);

    return ent;
}

}  // namespace ITD
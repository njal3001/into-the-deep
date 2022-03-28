#include "rocket.h"
#include <glm/gtx/vector_angle.hpp>
#include "mover.h"
#include "../maths/calc.h"
#include "tilemap.h"
#include "hurtable.h"
#include "explosion.h"
#include "collider.h"

namespace ITD
{
    Rocket::Rocket()
        : m_life_timer(life_time), m_max_speed(0.0f), m_target(nullptr), tracker(nullptr)
    {}

    void Rocket::awake()
    {
        Collider *col = get<Collider>();
        Mover *mov = get<Mover>();
        const glm::vec2 dir = Calc::normalize(mov->vel);
        col->set_rotation(glm::orientedAngle(glm::vec2(dir.x, -dir.y), glm::vec2(1.0f, 0.0f)));
        m_max_speed = glm::length(mov->vel);
    }

    void Rocket::update(const float elapsed)
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
                const glm::vec2 tracker_dir = glm::rotate(Calc::right, collider->get_rotation());
                tracker->set_pos(m_entity->get_pos() + tracker_dir * (tracker_width - collider_width) / 2.0f);
                tracker_collider->set_rotation(collider->get_rotation());

                std::vector<Collider*> in_range;
                tracker_collider->check_all(Mask::Enemy, &in_range);

                float min_dist = FLT_MAX;
                for (auto other : in_range)
                {
                    const float dist = collider->distance(*other);
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
                const glm::vec2 dir = Calc::normalize(m_target->get_pos() - entity()->get_pos());
                const float target_rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), Calc::right);
                collider->set_rotation(Calc::shortest_rotation_approach(collider->get_rotation(), target_rotation, rotation_multiplier * elapsed));
                facing = glm::rotate(Calc::right, collider->get_rotation());
            }
            else
            {
                facing = Calc::normalize(mov->vel);
            }

            mov->vel = Calc::approach(mov->vel, facing * m_max_speed, accel * elapsed);
        }
    }

    void Rocket::explode()
    {
        Collider *col = get<Collider>();
        Explosion::create(scene(), m_entity->get_pos() + col->get_bounds().center(), explosion_duration, 
                glm::vec2(explosion_width, explosion_height), col->get_rotation(), Mask::Enemy);
        tracker->destroy();
        m_entity->destroy();
    }

    void Rocket::render(Renderer *renderer)
    {
        Collider *col = get<Collider>();
        // Collider *tracker_collider = tracker->get<Collider>();
        // const Quadf& target_quad = tracker_collider->quad();
        // renderer->quad(target_quad.a, target_quad.b, target_quad.c, target_quad.d, Color::black);


        const Quadf& quad = col->quad();
        renderer->quad(quad.a, quad.b, quad.c, quad.d, Color::white);
    }

    Entity* Rocket::create(Scene *scene, const glm::vec2 &pos, const glm::vec2& vel)
    {
        Entity *ent = scene->add_entity(pos);
        Rocket *rocket = new Rocket();
        ent->add(rocket);

        Collider *col = new Collider(Rectf(glm::vec2(), glm::vec2(collider_width, collider_height)));
        col->collides_with = Mask::Solid | Mask::Enemy;
        col->trigger_only = true;
        col->on_collide = [](Collider *collider, Collider *other, const glm::vec2 &dir)
        {
            Rocket *rocket = collider->get<Rocket>();
            rocket->explode();

            return true;
        };

        ent->add(col);

        Mover *mov = new Mover();
        mov->vel = vel;
        ent->add(mov);

        Entity *tracker = scene->add_entity(pos);
        const glm::vec2 tracker_bl = col->get_bounds().center() - glm::vec2(tracker_width, tracker_height) / 2.0f;
        const glm::vec2 tracker_tr = tracker_bl + glm::vec2(tracker_width, tracker_height);

        Collider *tracker_collider = new Collider(Rectf(tracker_bl, tracker_tr));

        tracker->add(tracker_collider);
        rocket->tracker = tracker;

        return ent;
    }
}

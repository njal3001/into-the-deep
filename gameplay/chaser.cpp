#include "chaser.h"
#include <glm/gtx/vector_angle.hpp>
#include "player.h"
#include "mover.h"
#include "../maths/calc.h"
#include "hurtable.h"

namespace ITD
{
    Chaser::Chaser()
        : m_attached_to(nullptr), m_detach_recovery_timer(0.0f)
    {}

    bool Chaser::on_collide(Collider *other, const glm::vec2 &dir)
    {
        if (other->mask & Mask::Player && m_detach_recovery_timer <= 0.0f)
        {
            Player *player = other->get<Player>();
            if (player->attach(this))
            {
                m_attached_to = player;

                Mover *mover = get<Mover>();
                Collider *collider = get<Collider>();
                mover->vel = glm::vec2();
                collider->active = false;
                m_attack_timer = attack_time;
            }
        }

        return false;
    }

    void Chaser::detach()
    {
        m_attached_to = nullptr;
        Collider *collider = get<Collider>();
        collider->active = true;
        m_detach_recovery_timer = detach_recovery_time;
    }

    void Chaser::update(const float elapsed)
    {
        Collider *collider = get<Collider>();
        if (m_attached_to)
        {
            m_entity->set_pos(m_attached_to->entity()->get_pos());

            m_attack_timer -= elapsed;
            if (m_attack_timer <= 0.0f)
            {
                Hurtable *hurtable = m_attached_to->get<Hurtable>();
                hurtable->hurt(glm::vec2());
                m_attack_timer = attack_time;
            }
        }
        else if (m_detach_recovery_timer <= 0.0f)
        {
            Mover *mover = get<Mover>();
            glm::vec2 dir;
            float moving = 1.0f;

            auto pfirst = scene()->first<Player>();
            if (pfirst != scene()->end<Player>())
            {
                Player* player = (Player*)*pfirst;
                dir = Calc::normalize(player->entity()->get_pos() - entity()->get_pos());
            }
            else
            {
                dir = mover->vel;
                moving = 0.0f;
            }

            const glm::vec2 right = glm::vec2(1.0f, 0.0f);
            const float target_rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), right);
            collider->set_rotation(Calc::shortest_rotation_approach(collider->get_rotation(), target_rotation, rotation_multiplier * elapsed));

            const glm::vec2 facing = glm::rotate(right, collider->get_rotation());
            mover->vel = Calc::approach(mover->vel, facing * max_speed * moving, accel * elapsed);
        }

        m_detach_recovery_timer -= elapsed;
    }

    void Chaser::render(Renderer *renderer)
    {
        Collider *col = get<Collider>();

        const Quadf& quad = col->quad();
        renderer->quad(quad.a, quad.b, quad.c, quad.d, Color::red);
    }

    Entity* Chaser::create(Scene *scene, const glm::vec2& pos)
    {
        Entity *e = scene->add_entity(pos);
        e->add(new Chaser());

        Collider *c = new Collider(Rectf(glm::vec2(0.0f, 0.0f), glm::vec2(8.0f, 8.0f)));
        c->mask = Mask::Enemy;
        c->collides_with = Mask::Solid | Mask::Player | Mask::Enemy;
        c->on_collide = [](Collider *collider, Collider *other, const glm::vec2 &dir)
        {
            Chaser *chaser = collider->get<Chaser>();
            return chaser->on_collide(other, dir);
        };

        e->add(c);

        Mover *m = new Mover();
        e->add(m);

        Hurtable *h = new Hurtable();
        h->health = 2;
        h->on_hurt = [](Hurtable *self, const glm::vec2 &force)
        {
            Mover *mov = self->get<Mover>();
            mov->vel = force * hurt_knockback; 
        };
        e->add(h);


        return e;
    }
}

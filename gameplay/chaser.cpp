#include "chaser.h"
#include <glm/gtx/vector_angle.hpp>
#include "player.h"
#include "mover.h"
#include "../maths/calc.h"
#include "hurtable.h"

namespace ITD
{
    Chaser::Chaser()
    {}

    void Chaser::update(const float elapsed)
    {
        Collider *collider = get<Collider>();
        Mover *mover = get<Mover>();
        glm::vec2 dir;
        float moving = 1.0f;

        Node<Player> *pnode = scene()->first<Player>();
        if (pnode)
        {
            Player* player = pnode->data;
            dir = Calc::normalize(player->entity()->pos - entity()->pos);
        }
        else
        {
            dir = mover->vel;
            moving = 0.0f;
        }

        const glm::vec2 right = glm::vec2(1.0f, 0.0f);
        const float target_rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), right);
        collider->rotation = Calc::shortest_rotation_approach(collider->rotation, target_rotation, rotation_multiplier * elapsed);

        const glm::vec2 facing = glm::rotate(right, collider->rotation);
        mover->vel = Calc::approach(mover->vel, facing * max_speed * moving, accel * elapsed);
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
        e->add(c);

        Mover *m = new Mover();
        m->collider = c;
        m->collides_with |= Mask::Player | Mask::Enemy;
        m->on_hit = [](Mover *mover, Collider *other, const glm::vec2 &dir)
        {
            if (other->mask & Mask::Player)
            {
                Hurtable *hurtable = other->get<Hurtable>();
                hurtable->hurt(-dir);
            }

            return false;
        };

        e->add(m);

        Hurtable *h = new Hurtable();
        h->health = 2;
        h->on_hurt = [](Hurtable *self, const glm::vec2 &dir)
        {
            Mover *mov = self->get<Mover>();
            mov->vel = dir * hurt_knockback; 
        };
        e->add(h);


        return e;
    }
}

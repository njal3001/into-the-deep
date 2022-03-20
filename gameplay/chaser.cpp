#include "chaser.h"
#include <glm/gtx/vector_angle.hpp>
#include "player.h"
#include "mover.h"
#include "../maths/calc.h"
#include "hurtable.h"

namespace Uboat
{
    Chaser::Chaser()
    {}

    void Chaser::update(const float elapsed)
    {
        Collider *collider = get<Collider>();
        Mover *mover = get<Mover>();
        glm::vec2 dir;

        Node<Player> *pnode = scene()->first<Player>();
        if (pnode)
        {
            Player* player = pnode->data;
            dir = Calc::normalize(player->entity()->pos - entity()->pos);
            mover->vel = Calc::approach(mover->vel, dir * max_speed, accel * elapsed);
        }
        else
        {
            mover->vel = Calc::approach(mover->vel, glm::vec2(), accel * elapsed);
            dir = mover->vel;
        }

        const glm::vec2 right = glm::vec2(1.0f, 0.0f);
        const float target_rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), right);
        collider->rotation = Calc::shortest_rotation_approach(collider->rotation, target_rotation, rotation_multiplier * elapsed);
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

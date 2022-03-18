#include "chaser.h"
#include "player.h"
#include "mover.h"
#include "../maths/calc.h"

namespace Uboat
{
    void Chaser::update(const float elapsed)
    {
        Node<Player> *pnode = scene()->first<Player>();
        Player* player = pnode->data;

        Mover *mover = get<Mover>();
        if (player)
        {
            const glm::vec2 dir = Calc::normalize(player->entity()->pos - entity()->pos);
            mover->vel = Calc::approach(mover->vel, dir * 30.0f, 50.0f * elapsed);
        }
        else
        {
            mover->vel = Calc::approach(mover->vel, glm::vec2(), 50.0f * elapsed);
        }
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
        m->stop_mask |= Mask::Player | Mask::Enemy;
        e->add(m);

        return e;
    }
}

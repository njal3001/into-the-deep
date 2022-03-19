#include "bullet.h"
#include <glm/gtx/vector_angle.hpp>
#include "mover.h"
#include "../maths/calc.h"
#include "tilemap.h"
#include "hurtable.h"

namespace Uboat
{
    Bullet::Bullet()
        : m_life_timer(life_time)
    {}

    void Bullet::awake()
    {
        Collider *col = get<Collider>();
        Mover *mov = get<Mover>();
        const glm::vec2 dir = Calc::normalize(mov->vel);
        col->rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), glm::vec2(1.0f, 0.0f));
    }

    void Bullet::update(const float elapsed)
    {
        m_life_timer -= elapsed;
        if (m_life_timer <= 0.0f)
        {
            m_entity->destroy();
        }
        else
        {
            const glm::vec2 &pos = m_entity->pos;
            const Tilemap *map = scene()->map();
            const float pw = map->width() * 8.0f;
            const float ph = map->height() * 8.0f;
            if (pos.x < 0.0f || pos.x >= pw || pos.y < 0.0f || pos.y >= ph)
            {
                m_entity->destroy();
            }
        }
    }

    void Bullet::on_hit(Collider *other, const glm::vec2 &dir)
    {
        Hurtable *hurtable = other->get<Hurtable>();
        if (hurtable)
        {
            hurtable->hurt();
        }

        m_entity->destroy();
    }

    void Bullet::render(Renderer *renderer)
    {
        Collider *col = get<Collider>();

        const Quadf& quad = col->quad();
        renderer->quad(quad.a, quad.b, quad.c, quad.d, Color::white);
    }

    Entity* Bullet::create(Scene *scene, const glm::vec2 &pos, const glm::vec2& vel)
    {
        Entity *e = scene->add_entity(pos);
        e->add(new Bullet());

        Collider *c = new Collider(Rectf(glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 4.0f)));
        e->add(c);

        Mover *m = new Mover();
        m->stop_mask |= Mask::Enemy;
        m->collider = c;
        m->vel = vel;
        m->on_hit = [](Mover *mover, Collider *other, const glm::vec2 &dir)
        {
            Bullet *b = mover->get<Bullet>();
            b->on_hit(other, dir);
        };

        e->add(m);

        return e;
    }
}

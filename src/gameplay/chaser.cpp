#include "chaser.h"
#include <glm/gtx/vector_angle.hpp>
#include "../maths/calc.h"
#include "collider.h"
#include "explosion.h"
#include "hurtable.h"
#include "mover.h"
#include "player.h"

namespace ITD {

bool Chaser::on_collide(Collider *other, const glm::vec2 &dir)
{
    if (other->mask & Mask::Player)
    {
        explode();
        return true;
    }

    return false;
}

void Chaser::explode()
{
    Collider *col = get<Collider>();
    Explosion::create(scene(), m_entity->get_pos() + col->get_bounds().center(),
                      explosion_duration,
                      glm::vec2(explosion_width, explosion_height),
                      col->get_rotation(), Mask::Enemy | Mask::Player);
    m_entity->destroy();
}

void Chaser::update(const float elapsed)
{
    Collider *collider = get<Collider>();
    Mover *mover = get<Mover>();
    glm::vec2 dir;
    float moving = 1.0f;

    auto pfirst = scene()->first<Player>();
    if (pfirst != scene()->end<Player>())
    {
        Player *player = (Player *)*pfirst;
        dir =
            Calc::normalize(player->entity()->get_pos() - entity()->get_pos());
    }
    else
    {
        dir = mover->vel;
        moving = 0.0f;
    }

    const glm::vec2 right = glm::vec2(1.0f, 0.0f);
    const float target_rotation =
        glm::orientedAngle(glm::vec2(dir.x, -dir.y), right);
    collider->set_rotation(Calc::shortest_rotation_approach(
        collider->get_rotation(), target_rotation,
        rotation_multiplier * elapsed));

    const glm::vec2 facing = glm::rotate(right, collider->get_rotation());
    mover->vel = Calc::approach(mover->vel, facing * max_speed * moving,
                                accel * elapsed);
}

void Chaser::render(Renderer *renderer)
{
    Collider *col = get<Collider>();

    const Quadf &quad = col->quad();
    renderer->quad(quad.a, quad.b, quad.c, quad.d, Color::red);
}

Entity *Chaser::create(Scene *scene, const glm::vec2 &pos)
{
    Entity *e = scene->add_entity(pos);
    e->add(new Chaser());

    Collider *c =
        new Collider(Rectf(glm::vec2(0.0f, 0.0f), glm::vec2(8.0f, 8.0f)));
    c->mask = Mask::Enemy;
    c->collides_with = Mask::Solid | Mask::Player | Mask::Enemy;
    c->on_collide = [](Collider *collider, Collider *other,
                       const glm::vec2 &dir) {
        Chaser *chaser = collider->get<Chaser>();
        return chaser->on_collide(other, dir);
    };

    e->add(c);

    Mover *m = new Mover();
    e->add(m);

    Hurtable *h = new Hurtable();
    h->health = 1;
    h->on_hurt = [](Hurtable *self, const glm::vec2 &force) {
        Chaser *chaser = self->get<Chaser>();
        chaser->explode();
    };

    e->add(h);

    return e;
}

}  // namespace ITD

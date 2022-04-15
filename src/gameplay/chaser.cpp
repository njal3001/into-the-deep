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

void Chaser::update(float elapsed)
{
    Collider *collider = get<Collider>();
    Mover *mover = get<Mover>();
    float moving = 0.0f;

    Player *player = nullptr;

    auto pfirst = scene()->first<Player>();
    if (pfirst != scene()->end<Player>())
    {
        Player *player = (Player *)*pfirst;

        glm::vec2 player_diff =
            player->entity()->get_pos() - m_entity->get_pos();
        float player_distance = glm::length(player_diff);
        if (player_distance <= aggro_range)
        {
            moving = 1.0f;

            glm::vec2 dir = Calc::normalize(player->entity()->get_pos() -
                                            m_entity->get_pos());
            mover->rotate_towards(dir,
                                  Calc::TAU * rotation_multiplier * elapsed);
        }
    }

    mover->target_speed = max_speed * moving;

    collider->face_towards(mover->facing);
}

void Chaser::render(Renderer *renderer)
{
    Collider *col = get<Collider>();

    renderer->quad(col->quad(), Color::red);
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
    m->accel = accel;
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

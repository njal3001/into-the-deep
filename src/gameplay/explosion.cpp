#include "explosion.h"
#include "collider.h"
#include "hurtable.h"
#include "mover.h"
#include "content.h"

namespace ITD {

Explosion::Explosion(float duration)
    : m_duration_timer(duration)
{
}

void Explosion::awake()
{
    Sound *sfx = Content::find_sound("explosion");
    sfx->play();
}

void Explosion::update(float elapsed)
{
    m_duration_timer -= elapsed;
    if (m_duration_timer <= 0.0f)
    {
        m_entity->destroy();
    }
}

void Explosion::on_hit(Collider *other, const glm::vec2 &dir)
{
    Hurtable *hurtable = other->get<Hurtable>();
    if (hurtable)
    {
        if (hurtable->hurt(-dir))
        {
            scene()->freeze(0.05f);
        }
    }
}

void Explosion::render(Renderer *renderer)
{
    Collider *col = get<Collider>();
    renderer->quad(col->quad(), Color(255, 255, 0));
}

Entity *Explosion::create(Scene *scene, const glm::vec2 &pos, float duration,
                          const glm::vec2 &size, float rotation,
                          uint32_t hurt_mask)
{
    Entity *ent = scene->add_entity(pos);
    Explosion *explosion = new Explosion(duration);
    ent->add(explosion);

    Collider *col =
        new Collider(Rectf(-size / 2.0f, size / 2.0f), rotation, false);
    col->collides_with = hurt_mask;
    col->trigger_only = true;
    col->on_collide = [](Collider *collider, Collider *other,
                         const glm::vec2 &dir) {
        Explosion *exp = collider->get<Explosion>();
        exp->on_hit(other, dir);

        return true;
    };

    ent->add(col);

    return ent;
}

}  // namespace ITD

#include "factory.h"
#include "collider.h"
#include "ecs.h"

namespace ITD {

Entity *Factory::wall(Scene *scene, const glm::vec2 &pos)
{
    Entity *e = scene->add_entity(pos);
    Collider *c = new Collider(
        Rectf(glm::vec2(0.0f, 0.0f), glm::vec2(8.0f, 8.0f)), 0.0f, false);

    c->mask = Mask::Solid;
    e->add(c);
    return e;
}

}  // namespace ITD

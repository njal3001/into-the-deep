#include "playerhud.h"
#include "hurtable.h"
#include "player.h"

namespace ITD {

void PlayerHUD::render(Renderer *renderer)
{
    auto pfirst = scene()->first<Player>();
    if (pfirst != scene()->end<Player>())
    {
        const Player *player = (Player *)*pfirst;
        const Hurtable *hurtable = player->get<Hurtable>();

        Rectf world_bounds = scene()->world_bounds();
        glm::vec2 top_left = world_bounds.top_left();

        glm::vec2 size = glm::vec2(width, height);
        for (int i = 0; i < hurtable->health; i++)
        {
            // TODO: Don't hardcode screen height
            glm::vec2 pos =
                top_left + glm::vec2(offset_x + margin * i, -offset_y);
            renderer->rect(pos, pos + size, Color::green);
        }

        for (int i = 0; i < player->torpedo_ammo(); i++)
        {
            const glm::vec2 pos =
                top_left + glm::vec2(offset_x + margin * i, -offset_y - 10.0f);
            renderer->rect(pos, pos + size, Color::red);
        }
    }
}

Entity *PlayerHUD::create(Scene *scene)
{
    Entity *e = scene->add_entity(glm::vec2(0.0f, 0.0f));
    e->add(new PlayerHUD());

    return e;
}

}  // namespace ITD

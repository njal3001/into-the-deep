#include "playerhud.h"
#include "player.h"
#include "hurtable.h"

namespace Uboat
{
    void PlayerHUD::render(Renderer *renderer)
    {
        const Node<Player> *pnode = scene()->first<Player>();
        if (pnode)
        {
            const Player *player = pnode->data;
            const Hurtable *hurtable = player->get<Hurtable>();

            const glm::vec2 size = glm::vec2(width, height);
            for (int i = 0; i < hurtable->health; i++)
            {
                // TODO: Don't hardcode screen height
                const glm::vec2 pos = glm::vec2(offset_x + margin * i, 180.0f - offset_y);
                renderer->rect(pos, pos + size, Color::green);
            }
        }
    }

    Entity* PlayerHUD::create(Scene *scene)
    {
        Entity *e = scene->add_entity(glm::vec2(0.0f, 0.0f));
        e->add(new PlayerHUD());

        return e;
    }
}

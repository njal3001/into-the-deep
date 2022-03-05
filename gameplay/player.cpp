#include "player.h"
#include "collider.h"
#include "../input.h"
#include "../maths/calc.h"
#include <glm/gtx/vector_angle.hpp>

namespace Uboat
{
    void Player::update(const float elapsed)
    {
        glm::vec2 dir;

        const Input::Controller *controller = Input::controller();

        if (controller->active())
        {
            dir.x = controller->axes[0];
            dir.y = -controller->axes[1];
        }
        else
        {
            dir.x = Input::keyboard()->down[SDL_SCANCODE_RIGHT]
                - Input::keyboard()->down[SDL_SCANCODE_LEFT];

            dir.y = Input::keyboard()->down[SDL_SCANCODE_UP]
                - Input::keyboard()->down[SDL_SCANCODE_DOWN];
        }

        dir = Calc::normalize(dir);

        Collider *col = get<Collider>();
        if (dir.x != 0 || dir.y != 0)
        {
            col->rotation = glm::orientedAngle(glm::vec2(dir.x, -dir.y), glm::vec2(1.0f, 0.0f));
        }

        m_entity->pos += 70.0f * dir * elapsed;

        std::vector<Collider*> out;
        scene()->all(&out);

        for (auto other : out)
        {
            if (other != col)
            {
                const glm::vec2 push = col->push_out(*other);
                m_entity->pos += push;
            }
        }
    }

    void Player::render(Renderer *renderer)
    {
        Collider *col = get<Collider>();

        const Quadf& quad = col->get();
        renderer->quad(quad.a, quad.b, quad.c, quad.d, Color::black);
    }

    Entity* Player::create(Scene *scene, const glm::vec2& pos)
    {
        Entity* e = scene->add_entity(pos);
        e->add(new Player());

        Collider* c = new Collider(Rectf(glm::vec2(0.0f, 0.0f), glm::vec2(12.0f, 8.0f)));
        e->add(c);
        return e;
    }
}

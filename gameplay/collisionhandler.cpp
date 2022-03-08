#include "collisionhandler.h"
#include "collider.h"
#include "mover.h"
#include "ecs.h"
#include <memory>

namespace Uboat
{
    CollisionHandler::CollisionHandler()
        : m_scene(nullptr)
    {}

    void CollisionHandler::init(Scene *scene)
    {
        assert(!m_scene);
        m_scene = scene;

        // TODO: Initialize list of buckets
    }

    void CollisionHandler::update()
    {
        auto mover = m_scene->first<Mover>();
        while (mover)
        {
            auto col = mover->collider;
            auto ocol = m_scene->first<Collider>();

            while (ocol)
            {
                if (col != ocol && (mover->stop_mask & ocol->mask))
                {
                    const glm::vec2 push = col->push_out(*ocol);
                    if (push != glm::vec2())
                    {
                        col->entity()->pos += push;
                        col->invalidate_cache();
                    }
                }

                ocol = (Collider*)ocol->next();
            }

            mover = (Mover*)mover->next();
        }
    }
}

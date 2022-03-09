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
        auto mnode = m_scene->first<Mover>();
        while (mnode)
        {
            auto mover = mnode->data;
            auto col = mover->collider;

            auto onode = m_scene->first<Collider>();

            while (onode)
            {
                auto ocol = onode->data;
                if (col != ocol && (mover->stop_mask & ocol->mask))
                {
                    const glm::vec2 push = col->push_out(*ocol);
                    if (push != glm::vec2())
                    {
                        col->entity()->pos += push;
                        col->invalidate_cache();
                    }
                }

                onode = onode->next;
            }

            mnode = mnode->next;
        }
    }
}

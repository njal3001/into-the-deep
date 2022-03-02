#include "ecs.h"
#include <algorithm>
#include <vector>

namespace Uboat
{
    Entity::Entity(const glm::vec2& pos)
        : pos(pos), visible(true), m_scene(nullptr), m_alive(true)
    {}

    Entity::~Entity()
    {
        for (auto c : m_to_add)
        {
            delete c;
        }

        for (auto c : m_components)
        {
            delete c;
        }
    }

    bool Entity::alive() const
    {
        return m_alive;
    }

    Scene* Entity::scene() const
    {
        return m_scene;
    }

    void Entity::removed()
    {
        for (auto c : m_components)
        {
            m_scene->untrack_component(c);
        }

        m_scene = nullptr;
    }

    void Entity::destroy()
    {
        m_alive = false;
    }

    void Entity::update_lists()
    {
        {
            auto it = m_components.begin();

            while (it != m_components.end())
            {
                Component* c = *it;
                if (!c->m_alive)
                {
                    it = m_components.erase(it);

                    m_scene->untrack_component(c);
                    delete c;
                }
                else
                {
                    it++;
                }
            }
        }

        for (auto c : m_to_add)
        {
            c->m_entity = this;
            m_components.push_back(c);
            m_scene->track_component(c);
        }

        // Need to wake components after clearing because new components
        // can be added in awake function

        std::vector<Component*> added;
        added.insert(added.begin(), m_to_add.begin(), m_to_add.end());

        m_to_add.clear();

        for (auto c : added)
        {
            c->awake();
        }
    }
}

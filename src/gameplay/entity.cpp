#include <algorithm>
#include <vector>
#include "collider.h"
#include "ecs.h"

namespace ITD {

Entity::Entity(const glm::vec2 &pos)
    : m_pos(pos)
    , visible(true)
    , m_scene(nullptr)
    , m_alive(true)
{
}

void Entity::set_pos(const glm::vec2 &pos)
{
    m_pos = pos;

    Collider *collider = get<Collider>();
    if (collider)
    {
        collider->on_position_changed();
    }
}

glm::vec2 Entity::get_pos() const
{
    return m_pos;
}

void Entity::translate(const glm::vec2 &amount)
{
    m_pos += amount;
    Collider *collider = get<Collider>();
    if (collider)
    {
        collider->on_position_changed();
    }
}

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

Scene *Entity::scene() const
{
    return m_scene;
}

void Entity::removed()
{
    for (auto c : m_components)
    {
        m_scene->untrack_component(c);
        c->destroy();
        c->on_removed();

        c->m_entity = nullptr;
    }

    m_scene = nullptr;
}

void Entity::destroy()
{
    m_alive = false;
}

void Entity::update_lists()
{
    for (auto it = m_components.begin(); it != m_components.end();)
    {
        Component *c = *it;
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

    for (auto c : m_to_add)
    {
        c->m_entity = this;
        m_components.push_back(c);
        m_scene->track_component(c);
    }

    // Waking up components after all have been added
    std::vector<Component *> added;
    added.insert(added.begin(), m_to_add.begin(), m_to_add.end());

    m_to_add.clear();

    for (auto c : added)
    {
        c->awake();
    }
}

}  // namespace ITD

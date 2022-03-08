#include "ecs.h"
#include <string.h>

namespace Uboat
{
    Component::Component()
        : visible(true), m_type(0), m_alive(true), m_entity(nullptr), 
        m_next(nullptr), m_prev(nullptr)
    {}

    Component::~Component()
    {}

    bool Component::alive() const
    {
        return m_alive && m_entity->alive();
    }

    uint8_t Component::type() const
    {
        return m_type;
    }

    Entity* Component::entity() const
    {
        return m_entity;
    }

    Scene* Component::scene() const
    {
        return (m_entity ? m_entity->scene() : nullptr);
    }

    Component* Component::next() const
    {
        return m_next;
    }

    Component* Component::prev() const
    {
        return m_prev;
    }

    void Component::destroy()
    {
        m_alive = false;
    }

    void Component::awake()
    {}

    void Component::update(const float elapsed)
    {}

    void Component::render(Renderer *renderer)
    {}
}

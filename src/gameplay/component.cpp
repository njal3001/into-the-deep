#include <algorithm>
#include <string>
#include "ecs.h"

namespace ITD {

Component::Component()
    : visible(true)
    , m_type(0)
    , m_alive(true)
    , m_entity(nullptr)
{
}

Component::~Component()
{
}

bool Component::alive() const
{
    return m_alive && m_entity->alive();
}

uint8_t Component::type() const
{
    return m_type;
}

Entity *Component::entity() const
{
    return m_entity;
}

Scene *Component::scene() const
{
    return (m_entity ? m_entity->scene() : nullptr);
}

void Component::destroy()
{
    m_alive = false;
}

void Component::notify_on_death(Component *listener)
{
    m_death_listeners.push_back(listener);
}

void Component::stop_notify_on_death(Component *listener)
{
    auto position = std::find(m_death_listeners.begin(), m_death_listeners.end(), listener);

    ITD_ASSERT(position != m_death_listeners.end(), "Tried to remove death listener that was not added");

    m_death_listeners.erase(position);
}

void Component::death_notification(Component *dead)
{
}

void Component::awake()
{
}

void Component::on_removed()
{
    for (auto listener : m_death_listeners)
    {
        listener->death_notification(this);
    }
}

void Component::update(float elapsed)
{
}

void Component::render(Renderer *renderer)
{
}

}  // namespace ITD

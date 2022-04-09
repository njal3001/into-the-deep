#include <string.h>
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

void Component::awake()
{
}

void Component::on_removed()
{
}

void Component::update(const float elapsed)
{
}

void Component::render(Renderer *renderer)
{
}

}  // namespace ITD

#include <algorithm>
#include "ecs.h"
#include "tilemap.h"

namespace ITD {

Scene::Scene(Tilemap *map)
    : m_tilemap(map)
    , m_freeze_timer(0.0f)
    , m_debug(false)
{
    map->fill_scene(this);
    m_collision_handler.init(this);
}

Scene::~Scene()
{
    for (auto iter = m_to_add.begin(); iter != m_to_add.end();)
    {
        delete *iter;
        iter = m_to_add.erase(iter);
    }

    for (auto iter = m_entities.begin(); iter != m_entities.end();)
    {
        delete *iter;
        iter = m_to_add.erase(iter);
    }
}

Entity *Scene::add_entity(const glm::vec2 &pos)
{
    Entity *entity = new Entity(pos);
    entity->m_scene = this;

    m_to_add.insert(m_to_add.end(), entity);

    return entity;
}

void Scene::track_component(Component *component)
{
    m_components[component->type()].push_back(component);
    component->m_iterator = --m_components[component->type()].end();
}

void Scene::untrack_component(Component *component)
{
    assert(component->scene() == this);
    m_components[component->type()].erase(component->m_iterator);
}

void Scene::update(float elapsed)
{
    if (m_freeze_timer > 0)
    {
        m_freeze_timer = std::max(0.0f, m_freeze_timer - elapsed);
        return;
    }

    update_lists();

    for (auto ent : m_entities)
    {
        ent->update_lists();
    }

    for (size_t i = 0; i < Component::Types::count(); i++)
    {
        if (s_prop_masks[i] & Property::Updatable)
        {
            for (auto comp : m_components[i])
            {
                comp->update(elapsed);
            }
        }
    }

    m_collision_handler.update();
}

void Scene::update_lists()
{
    for (auto iter = m_entities.begin(); iter != m_entities.end();)
    {
        if (!(*iter)->m_alive)
        {
            (*iter)->removed();
            delete *iter;
            iter = m_entities.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    for (auto iter = m_to_add.begin(); iter != m_to_add.end();)
    {
        m_entities.push_back(*iter);
        (*iter)->m_iterator = --m_entities.end();
        iter = m_entities.erase(iter);
    }
}

void Scene::render(Renderer *renderer)
{
    m_tilemap->render(renderer);

    if (m_debug)
    {
        m_collision_handler.render_dynamic_buckets(renderer);
    }

    for (size_t i = 0; i < Component::Types::count(); i++)
    {
        if (s_prop_masks[i] & Property::Renderable)
        {
            for (auto comp : m_components[i])
            {
                if (comp->visible && comp->entity()->visible)
                {
                    comp->render(renderer);
                }
            }
        }
    }

    if (m_debug)
    {
        m_collision_handler.render_collider_outlines(renderer);
    }
}

void Scene::render_hud(Renderer *renderer)
{
    for (size_t i = 0; i < Component::Types::count(); i++)
    {
        if (s_prop_masks[i] & Property::HUD)
        {
            for (auto comp : m_components[i])
            {
                if (comp->visible && comp->entity()->visible)
                {
                    comp->render(renderer);
                }
            }
        }
    }
}

void Scene::freeze(const float amount)
{
    m_freeze_timer += amount;
}

const Tilemap *Scene::map() const
{
    return m_tilemap;
}

CollisionHandler *Scene::collision_handler()
{
    return &m_collision_handler;
}

void Scene::toggle_debug_mode()
{
    m_debug = !m_debug;
}

}  // namespace ITD

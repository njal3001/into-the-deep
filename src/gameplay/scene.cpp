#include <algorithm>
#include "ecs.h"
#include "tilemap.h"

namespace ITD {

Scene::EntityRef::EntityRef()
    : version(1)
    , entity(nullptr)
{
}

Scene::Scene(Tilemap *map, const Rectf &world_bounds)
    : m_tilemap(map)
    , m_freeze_timer(0.0f)
    , m_world_bounds(world_bounds)
    , m_debug(false)
    , m_entity_registry_tail(0)
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
        (*iter)->removed();
        delete *iter;
        iter = m_entities.erase(iter);
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

Entity *Scene::get_entity(uint32_t id)
{
    uint16_t index = id & 0xFFFF;
    uint16_t version = (id >> 16) & 0xFFFF;

    EntityRef *ref = &m_entity_registry[index];

    if (ref->version != version)
    {
        return nullptr;
    }

    return ref->entity;
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
        Entity *ent = *iter;
        if (!ent->m_alive)
        {
            uint16_t index = ent->m_id & 0xFFFF;
            EntityRef *ref = &m_entity_registry[index];
            ref->version++;
            ref->entity = nullptr;

            if (index == m_entity_registry_tail - 1)
            {
                m_entity_registry_tail--;
            }
            else
            {
                m_entity_freelist.push_back(index);
            }

            ent->removed();
            delete ent;
            iter = m_entities.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    for (auto iter = m_to_add.begin(); iter != m_to_add.end();)
    {
        Entity *ent = *iter;
        m_entities.push_back(ent);
        ent->m_iterator = --m_entities.end();
        iter = m_entities.erase(iter);

        ITD_ASSERT(m_entity_registry_tail < max_entities ||
                       m_entity_freelist.size() > 0,
                   "Exceeded max entities");

        uint16_t index;
        if (m_entity_registry_tail < max_entities)
        {
            index = m_entity_registry_tail;
            m_entity_registry_tail++;
        }
        else
        {
            index = m_entity_freelist.back();
            m_entity_freelist.pop_back();
        }

        EntityRef *ref = &m_entity_registry[index];
        ref->entity = ent;

        ent->m_id = (ref->version << 16) | index;
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

void Scene::freeze(float amount)
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

Rectf Scene::world_bounds() const
{
    return m_world_bounds;
}

void Scene::toggle_debug_mode()
{
    m_debug = !m_debug;
}

}  // namespace ITD

#include "ecs.h"
#include <algorithm>
#include "tilemap.h"

namespace ITD
{
    Scene::Scene(Tilemap *map)
        : m_tilemap(map), m_freeze_timer(0.0f)
    {
        map->fill_scene(this);
        m_collision_handler.init(this);
    }

    Scene::~Scene()
    {
        {
            auto addnode = m_to_add.head;
            while (addnode)
            {
                auto next = addnode->next;
                auto ent = addnode->data;
                delete ent;
                addnode = next;
            }
        }

        {
            auto enode = m_entities.head;
            while (enode)
            {
                auto next = enode->next;
                auto ent = enode->data;
                delete ent;
                enode = next;
            }
        }
    }

    Entity* Scene::add_entity(const glm::vec2& pos)
    {
        Entity* entity = new Entity(pos);
        entity->m_scene = this;

        m_to_add.insert(&entity->m_node);

        return entity;
    }

    void Scene::track_component(Component* component)
    {
        m_components[component->type()].insert(&component->m_node);
    }

    void Scene::untrack_component(Component* component)
    {
        assert(component->scene() == this);
        m_components[component->type()].remove(&component->m_node);
    }

    void Scene::update(float elapsed)
    {
        if (m_freeze_timer > 0)
        {
            m_freeze_timer = std::max(0.0f, m_freeze_timer - elapsed);
            return;
        }

        update_lists();

        auto enode = m_entities.head;
        while (enode)
        {
            auto ent = enode->data;
            ent->update_lists();
            enode = enode->next;
        }

        for (size_t i = 0; i < Component::Types::count(); i++)
        {
            if (s_prop_masks[i] & Property::Updatable)
            {
                auto cnode = m_components[i].head;
                while (cnode)
                {
                    auto comp = cnode->data;
                    if (comp->alive())
                    {
                        comp->update(elapsed);
                    }

                    cnode = cnode->next;
                }
            }
        }

        m_collision_handler.update();
    }

    void Scene::update_lists()
    {
        {
            auto enode = m_entities.head;
            while (enode)
            {
                auto next = enode->next;
                auto ent = enode->data;
                if (!ent->m_alive)
                {
                    m_entities.remove(enode);
                    ent->removed();
                    delete ent;
                }

                enode = next;
            }
        }

        {
            auto addnode = m_to_add.head;
            while (addnode)
            {
                auto next = addnode->next;
                m_to_add.remove(addnode);
                m_entities.insert(addnode);

                addnode = next;
            }
        }
    }

    void Scene::render(Renderer *renderer)
    {
        m_tilemap->render(renderer);

        for (size_t i = 0; i < Component::Types::count(); i++)
        {
            if (s_prop_masks[i] & Property::Renderable)
            {
                auto cnode = m_components[i].head;
                while (cnode)
                {
                    auto comp = cnode->data;
                    if (comp->alive() && comp->visible && comp->entity()->visible)
                    {
                        comp->render(renderer);
                    }

                    cnode = cnode->next;
                }
            }
        }
    }

    void Scene::render_hud(Renderer *renderer)
    {
        for (size_t i = 0; i < Component::Types::count(); i++)
        {
            if (s_prop_masks[i] & Property::HUD)
            {
                auto cnode = m_components[i].head;
                while (cnode)
                {
                    auto comp = cnode->data;
                    if (comp->alive() && comp->visible && comp->entity()->visible)
                    {
                        comp->render(renderer);
                    }

                    cnode = cnode->next;
                }
            }
        }
    }

    void Scene::freeze(const float amount)
    {
        m_freeze_timer += amount;
    }

    const Tilemap* Scene::map() const
    {
        return m_tilemap;
    }

    CollisionHandler* Scene::collision_handler()
    {
        return &m_collision_handler;
    }
}

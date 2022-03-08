#include "ecs.h"
#include <algorithm>
#include "tilemap.h"

namespace Uboat
{
    Scene::Scene(Tilemap *map)
        : m_tilemap(map)
    {
        map->fill_scene(this);
        m_collision_handler.init(this);
    }

    Scene::~Scene()
    {
        Entity *to_add = m_to_add.head;
        while (to_add)
        {
            Entity *next = to_add->m_next;
            delete to_add;
            to_add = next;
        }

        Entity *ent = m_entities.head;
        while (ent)
        {
            Entity *next = ent->m_next;
            delete ent;
            ent = next;
        }
    }

    Entity* Scene::add_entity(const glm::vec2& pos)
    {
        Entity* entity = new Entity(pos);
        entity->m_scene = this;

        m_to_add.insert(entity);

        return entity;
    }

    void Scene::track_component(Component* component)
    {
        m_components[component->type()].insert(component);
    }

    void Scene::untrack_component(Component* component)
    {
        assert(component->scene() == this);
        m_components[component->type()].remove(component);
    }

    void Scene::update(float elapsed)
    {
        update_lists();

        auto ent = m_entities.head;
        while (ent)
        {
            ent->update_lists();
            ent = ent->m_next;
        }

        for (size_t i = 0; i < Component::Types::count(); i++)
        {
            if (s_prop_masks[i] & Property::Updatable)
            {
                auto comp = m_components[i].head;
                while (comp)
                {
                    if (comp->alive())
                    {
                        comp->update(elapsed);
                    }

                    comp = comp->m_next;
                }
            }
        }

        m_collision_handler.update();
    }

    void Scene::update_lists()
    {
        Entity *ent = m_entities.head;
        while (ent)
        {
            Entity *next = ent->m_next;
            if (!ent->m_alive)
            {
                m_entities.remove(ent);
                ent->removed();
                delete ent;
            }

            ent = next;
        }

        Entity *to_add = m_to_add.head;
        while (to_add)
        {
            Entity *next = to_add->m_next;

            m_to_add.remove(to_add);
            m_entities.insert(to_add);

            to_add = next;
        }
    }

    void Scene::render(Renderer *renderer)
    {
        m_tilemap->render(renderer);

        for (size_t i = 0; i < Component::Types::count(); i++)
        {
            if (s_prop_masks[i] & Property::Renderable)
            {
                auto comp = m_components[i].head;
                while (comp)
                {
                    if (comp->alive() && comp->visible && comp->entity()->visible)
                    {
                        comp->render(renderer);
                    }

                    comp = comp->m_next;
                }
            }
        }
    }
}

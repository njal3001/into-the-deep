#pragma once
#include <list>
#include <vector>
#include "../debug.h"
#include "../graphics/renderer.h"
#include "collisionhandler.h"

namespace ITD {

class Entity;
class Scene;

struct Property {
    static constexpr uint8_t None = 0;
    static constexpr uint8_t Updatable = 1;
    static constexpr uint8_t Renderable = 1 << 1;
    static constexpr uint8_t HUD = 1 << 2;
};

class Component
{
    friend class Scene;
    friend class Entity;

public:
    bool visible;
    uint32_t prop_mask;

private:
    uint8_t m_type;
    bool m_alive;

    std::list<Component *>::iterator m_iterator;

    class Types
    {
        inline static uint8_t s_counter = 0;

    public:
        static uint8_t count()
        {
            return s_counter;
        }

        template <class T>
        static uint8_t id()
        {
            static const uint8_t val = s_counter++;
            return val;
        }
    };

protected:
    Entity *m_entity;

public:
    Component();
    virtual ~Component();

    bool alive() const;
    uint8_t type() const;

    Entity *entity() const;
    Scene *scene() const;

    template <class T>
    T *get() const;

    virtual void destroy();

    virtual void update(float elapsed);
    virtual void render(Renderer *renderer);

protected:
    virtual void awake();
    virtual void on_removed();
};

class Entity
{
    friend class Scene;

public:
    bool visible;

private:
    glm::vec2 m_pos;

    Scene *m_scene;
    std::vector<Component *> m_components;
    std::vector<Component *> m_to_add;

    bool m_alive;

    std::list<Entity *>::iterator m_iterator;

    uint32_t m_id;

public:
    Entity(const glm::vec2 &pos);
    ~Entity();

    void set_pos(const glm::vec2 &pos);
    glm::vec2 get_pos() const;
    void translate(const glm::vec2 &amount);

    bool alive() const;
    Scene *scene() const;

    uint32_t id() const;

    template <class T>
    void add(T *component);

    template <class T>
    T *get() const;

    void remove(Component *component);

    void destroy();

private:
    void removed();
    void update_lists();
};

class Tilemap;

class Scene
{
public:
    static const uint16_t max_component_types = 256;
    static const uint16_t max_entities = 1024;

private:
    struct EntityRef
    {
        uint16_t version;
        Entity *entity;

        EntityRef();
    };

    // TODO: Shift away from linked lists
    std::list<Entity *> m_entities;
    std::list<Entity *> m_to_add;
    std::list<Component *> m_components[max_component_types];

    EntityRef m_entity_registry[max_entities];
    uint16_t m_entity_registry_tail;
    std::vector<uint16_t> m_entity_freelist;

    Tilemap *m_tilemap;
    CollisionHandler m_collision_handler;

    float m_freeze_timer;

    Rectf m_world_bounds;

    bool m_debug;


    static inline uint8_t s_prop_masks[max_component_types] = {Property::None};

public:
    Scene(Tilemap *map, const Rectf &world_bounds);
    ~Scene();

    template <class T>
    static void register_component(uint8_t prop_mask = Property::None);

    Entity *add_entity(const glm::vec2 &pos);
    void remove_entity(Entity *entity);

    Entity *get_entity(uint32_t id);

    void track_component(Component *component);
    void untrack_component(Component *component);

    void update(float elapsed);
    void render(Renderer *renderer);
    void render_hud(Renderer *renderer);

    template <class T>
    typename std::list<Component *>::iterator first();

    template <class T>
    typename std::list<Component *>::iterator end();

    void freeze(float amount);

    const Tilemap *map() const;
    CollisionHandler *collision_handler();

    Rectf world_bounds() const;
    void toggle_debug_mode();

private:
    void update_lists();
};

template <class T>
T *Component::get() const
{
    ITD_ASSERT(m_entity, "Component must have an entity");
    return m_entity->get<T>();
}

template <class T>
void Entity::add(T *component)
{
    ITD_ASSERT(m_scene, "Entity must be part of a scene");

    uint8_t type = Component::Types::id<T>();
    component->m_type = type;

    m_to_add.push_back(component);
}

template <class T>
T *Entity::get() const
{
    uint8_t type = Component::Types::id<T>();
    for (auto c : m_components)
    {
        if (c->m_type == type)
        {
            return (T *)c;
        }
    }

    return nullptr;
}

template <class T>
void Scene::register_component(uint8_t prop_mask)
{
    uint32_t id = Component::Types::id<T>();
    s_prop_masks[id] = prop_mask;
}

template <class T>
typename std::list<Component *>::iterator Scene::first()
{
    uint8_t type = Component::Types::id<T>();
    return m_components[type].begin();
}

template <class T>
typename std::list<Component *>::iterator Scene::end()
{
    uint8_t type = Component::Types::id<T>();
    return m_components[type].end();
}

}  // namespace ITD

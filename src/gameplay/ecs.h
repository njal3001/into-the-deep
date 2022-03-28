#pragma once
#include <assert.h>
#include <vector>
#include <list>
#include "../graphics/renderer.h"
#include "collisionhandler.h"

namespace ITD
{
    #define MAX_COMPONENT_TYPES 256

    class Entity;
    class Scene;
    
    struct Property
    {
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

        std::list<Component*>::iterator m_iterator;

        class Types
        {
            inline static uint8_t s_counter = 0;

        public:
            static uint8_t count()
            {
                return s_counter;
            }

            template<class T>
            static uint8_t id()
            {
                static const uint8_t val = s_counter++;
                return val;
            }
        };

    protected:
        Entity* m_entity;

    public:
        Component();
        virtual ~Component();

        bool alive() const;
        uint8_t type() const;

        Entity* entity() const;
        Scene* scene() const;

        template <class T>
        T* get() const;

        virtual void destroy();

        virtual void update(const float elapsed);
        virtual void render(Renderer* renderer);

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

        Scene* m_scene;
        std::vector<Component*> m_components;
        std::vector<Component*> m_to_add;

        bool m_alive;

        std::list<Entity*>::iterator m_iterator;

    public:
        Entity(const glm::vec2& pos);
        ~Entity();

        void set_pos(const glm::vec2 &pos);
        glm::vec2 get_pos() const;
        void translate(const glm::vec2 &amount);

        bool alive() const;
        Scene* scene() const;

        template <class T>
        void add(T* component);

        template <class T>
        T* get() const;

        void remove(Component* component);

        void destroy();

    private:
        void removed();
        void update_lists();
    };

    class Tilemap;

    class Scene
    {
    private:
        std::list<Entity*> m_entities;
        std::list<Entity*> m_to_add;
        std::list<Component*> m_components[MAX_COMPONENT_TYPES];

        Tilemap *m_tilemap;
        CollisionHandler m_collision_handler;
        float m_freeze_timer;

        static inline uint8_t s_prop_masks[MAX_COMPONENT_TYPES] = { Property::None };

    public:
        Scene(Tilemap *map);
        ~Scene();

        template <class T>
        static void register_component(const uint8_t prop_mask = Property::None);

        Entity* add_entity(const glm::vec2& pos);
        void remove_entity(Entity *entity);

        void track_component(Component *component);
        void untrack_component(Component *component);

        void update(float elapsed);
        void render(Renderer *renderer);
        void render_hud(Renderer *renderer);

        template<class T>
        typename std::list<Component*>::iterator first();

        template<class T>
        typename std::list<Component*>::iterator end();

        void freeze(const float amount);

        const Tilemap* map() const;
        CollisionHandler* collision_handler();

    private:
        void update_lists();
    };

    template <class T>
    T* Component::get() const
    {
        assert(m_entity);
        return m_entity->get<T>();
    }

    template <class T>
    void Entity::add(T *component)
    {
        assert(m_scene);

        const uint8_t type = Component::Types::id<T>();
        component->m_type = type;

        m_to_add.push_back(component);
    }

    template <class T>
    T* Entity::get() const
    {
        const uint8_t type = Component::Types::id<T>();
        for (auto c : m_components)
        {
            if (c->m_type == type)
            {
                return (T*)c;
            }
        }

        return nullptr;
    }

    template <class T>
    void Scene::register_component(const uint8_t prop_mask)
    {
        const uint32_t id = Component::Types::id<T>();
        s_prop_masks[id] = prop_mask;
    }

    template<class T>
    typename std::list<Component*>::iterator Scene::first()
    {
        const uint8_t type = Component::Types::id<T>();
        return m_components[type].begin();
    }

    template<class T>
    typename std::list<Component*>::iterator Scene::end()
    {
        const uint8_t type = Component::Types::id<T>();
        return m_components[type].end();
    }
}

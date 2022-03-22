#pragma once
#include <assert.h>
#include <vector>
#include "../graphics/renderer.h"
#include "collisionhandler.h"
#include "pool.h"

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

        Node<Component> m_node;
        Component *m_next;
        Component *m_prev;

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

        Component* next() const;
        Component* prev() const;

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
        glm::vec2 pos;
        bool visible;

    private:
        Scene* m_scene;
        std::vector<Component*> m_components;
        std::vector<Component*> m_to_add;

        bool m_alive;

        Entity *m_next;
        Entity *m_prev;
        Node<Entity> m_node;

    public:
        Entity(const glm::vec2& pos);
        ~Entity();

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
        Pool<Entity> m_entities;
        Pool<Entity> m_to_add;
        Pool<Component> m_components[MAX_COMPONENT_TYPES];
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
        Node<T>* first() const;

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
    Node<T>* Scene::first() const
    {
        const uint8_t type = Component::Types::id<T>();
        return (Node<T>*)m_components[type].head;
    }
}

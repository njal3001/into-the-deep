#pragma once
#include <assert.h>
#include <vector>
#include "../graphics/renderer.h"

namespace Uboat
{
    #define MAX_COMPONENT_TYPES 256

    class Entity;
    class Scene;
    
    struct Property
    {
        static constexpr uint8_t None = 0;
        static constexpr uint8_t Updatable = 1;
        static constexpr uint8_t Renderable = 1 << 1;
    };

    template <class T>
    struct Properties
    {
        static uint8_t get();
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

        Component *m_next;
        Component *m_prev;

        class Types
        {
            inline static uint8_t s_counter = 0;
            inline static uint8_t s_prop_masks[MAX_COMPONENT_TYPES] = { Property::None };

        public:
            static uint8_t count()
            {
                return s_counter;
            }

            template<class T>
            static uint8_t id()
            {
                static const uint8_t val = s_counter++;
                s_prop_masks[val] = Properties<T>::get();
                return val;
            }

            static uint8_t prop_mask(const uint8_t id)
            {
                return s_prop_masks[id];
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

        virtual void awake();
        virtual void destroy();

        virtual void update(const float elapsed);
        virtual void render(Renderer* renderer);
    };

    template<>
    struct Properties<Component>
    {
        static uint8_t get()
        {
            return Property::None;
        }
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
        template<class T>
        struct Pool
        {
            T* head = nullptr;
            T* tail = nullptr;

            void insert(T *instance);
            void remove(T *instance);
        };

        Pool<Entity> m_entities;
        Pool<Entity> m_to_add;
        Pool<Component> m_components[MAX_COMPONENT_TYPES];
        Tilemap *m_tilemap;

    public:
        Scene(Tilemap *map);
        ~Scene();

        Entity* add_entity(const glm::vec2& pos);
        void remove_entity(Entity *entity);

        void track_component(Component *component);
        void untrack_component(Component *component);

        void update(float elapsed);
        void render(Renderer *renderer);

        template<class T>
        T* first() const;

        template<class T>
        void all(std::vector<T*> *out) const;

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
            if (c->alive() && c->m_type == type)
            {
                return (T*)c;
            }
        }

        return nullptr;
    }

    template<class T>
    T* Scene::first() const
    {
        const uint8_t type = Component::Types::id<T>();
        return (T*)m_components[type].head;
    }

    template<class T>
    void Scene::Pool<T>::insert(T *instance)
    {
        if (tail)
        {
            tail->m_next = instance;
            instance->m_prev = tail;
            tail = instance;
        }
        else
        {
            tail = head = instance;
            instance->m_prev = instance->m_next = nullptr;
        }
    }

    template<class T>
    void Scene::Pool<T>::remove(T *instance)
    {
        if (instance->m_prev)
            instance->m_prev->m_next = instance->m_next;

        if (instance->m_next)
            instance->m_next->m_prev = instance->m_prev;

        if (head == instance)
            head = instance->m_next;

        if (tail == instance)
            tail = instance->m_prev;

        instance->m_next = instance->m_prev = nullptr;
    }
}

#pragma once
#include <assert.h>
#include <vector>
#include "../graphics/renderer.h"

namespace Uboat
{
    class Entity;
    class Scene;

    class Component
    {
        friend class Scene;
        friend class Entity;

    public:
        bool visible;

    private:
        uint8_t m_type;
        bool m_alive;

        class Types
        {
            inline static uint8_t s_counter = 0;

        public:
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

        virtual void awake();
        virtual void destroy();

        virtual void update(const float elapsed);
        virtual void render(Renderer* renderer);
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
    public:
        static constexpr int max_component_types = 256;

    private:
        std::vector<Entity*> m_entities;
        std::vector<Entity*> m_to_add;
        std::vector<Component*> m_components[max_component_types];
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

        size_t tile_size() const;
        size_t width() const;
        size_t height() const;

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
        auto& c_vec = m_components[type];

        for (auto c : c_vec)
        {
            if (c->alive())
            {
                return (T*)c;
            }
        }

        return nullptr;
    }

    template<class T>
    void Scene::all(std::vector<T*>* out) const
    {
        const uint8_t type = Component::Types::id<T>();
        auto& c_vec = m_components[type];

        for (auto c : c_vec)
        {
            if (c->alive())
            {
                out->push_back((T*)c);
            }
        }
    }
}

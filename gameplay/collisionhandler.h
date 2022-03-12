#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../maths/shapes.h"

namespace Uboat
{
    class Scene;
    class Collider;

    class CollisionHandler
    {
    private:
        Scene *m_scene;
        std::vector<std::vector<Collider*>> m_buckets;
        size_t m_grid_width;
        size_t m_grid_height;

    public:
        CollisionHandler();

        void init(Scene *scene);

        void update_buckets(Collider *collider);
        void add_bucket(Collider *collider, const size_t bx, const size_t by);
        void remove_bucket(Collider *collider, const size_t bx, const size_t by);
        void remove(Collider *collider);

        void update();

    private:
        glm::ivec2 bucket_index(const glm::vec2& pos);
        Recti bucket_box(const Rectf& bbox);
        void update_all_buckets();
    };
}

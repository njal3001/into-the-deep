#pragma once
#include <glm/glm.hpp>
#include <list>
#include <vector>
#include "../maths/shapes.h"

namespace ITD {

class Scene;
class Collider;

class CollisionHandler
{
private:
    static constexpr size_t collision_iterations = 1;
    static constexpr float collision_elasticity = 0.01f;

    Scene *m_scene;
    std::vector<std::vector<Collider *>> m_buckets;
    size_t m_grid_width;
    size_t m_grid_height;

    std::list<Collider *> m_dynamic_colliders;

public:
    CollisionHandler();

    void init(Scene *scene);

    void register_dynamic(Collider *collider);
    void deregister_dynamic(Collider *collider);

    void update_buckets(Collider *collider);
    void add_bucket(Collider *collider, const size_t bx, const size_t by);
    void remove_bucket(Collider *collider, const size_t bx, const size_t by);
    void remove(Collider *collider);

    void update();
    Collider *check(Collider *collider, const uint32_t mask);
    void check_all(Collider *collider, const uint32_t mask,
                   std::vector<Collider *> *out);

private:
    glm::ivec2 bucket_index(const glm::vec2 &pos);
    Recti bucket_box(const Rectf &bbox);
    void update_all_buckets();
    bool valid_bucket_index(const size_t bx, const size_t by);
};

}  // namespace ITD

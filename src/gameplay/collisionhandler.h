#pragma once
#include <glm/glm.hpp>
#include <list>
#include <vector>
#include "../graphics/renderer.h"
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
    void add_bucket(Collider *collider, size_t bx, size_t by);
    void remove_bucket(Collider *collider, size_t bx, size_t by);
    void remove(Collider *collider);

    void update();
    Collider *check(Collider *collider, uint32_t mask);
    void check_all(Collider *collider, uint32_t mask,
                   std::vector<Collider *> *out);

    void render_dynamic_buckets(Renderer *renderer);
    void render_collider_outlines(Renderer *renderer);

private:
    glm::ivec2 bucket_index(const glm::vec2 &pos);
    Recti bucket_box(const Rectf &bbox);
    void update_all_buckets();
    bool valid_bucket_index(size_t bx, size_t by);
};

}  // namespace ITD

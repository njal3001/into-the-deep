#include "collisionhandler.h"
#include "collider.h"
#include "mover.h"
#include "ecs.h"
#include "tilemap.h"
#include <memory>
#include "../maths/calc.h"

namespace Uboat
{
    CollisionHandler::CollisionHandler()
        : m_scene(nullptr)
    {}

    void CollisionHandler::init(Scene *scene)
    {
        assert(!m_scene);
        m_scene = scene;

        const Tilemap *map = m_scene->map();
        m_grid_width = std::ceil(map->width() / 2.0f);
        m_grid_height = std::ceil(map->height() / 2.0f);

        m_buckets.resize(m_grid_width * m_grid_height);
    }

    void CollisionHandler::update_buckets(Collider *collider)
    {
        const Rectf& bbox = collider->bbox();
        const Recti buc_box = bucket_box(bbox);

        const Recti& prev_buc_box = collider->m_bucket_box;

        // Remove from previous buckets
        if (collider->m_in_bucket)
        {
            const glm::ivec2& prev_bl = prev_buc_box.bl;
            const glm::ivec2& prev_tr = prev_buc_box.tr;

            for (int y = prev_bl.y; y <= prev_tr.y; y++)
            {
                for (int x = prev_bl.x; x <= prev_tr.x; x++)
                {
                    if (!buc_box.contains(glm::ivec2(x, y)))
                    {
                        remove_bucket(collider, x, y);
                    }
                }
            }
        }

        // Add to new buckets
        const glm::ivec2& bl = buc_box.bl;
        const glm::ivec2& tr = buc_box.tr;

        for (int y = bl.y; y <= tr.y; y++)
        {
            for (int x = bl.x; x <= tr.x; x++)
            {
                if (!collider->m_in_bucket || !prev_buc_box.contains(glm::ivec2(x, y)))
                {
                    add_bucket(collider, x, y);
                }
            }
        }

        collider->m_bucket_box = buc_box;
        collider->m_in_bucket = true;
    }

    void CollisionHandler::add_bucket(Collider *collider, const size_t bx, const size_t by)
    {
        if (valid_bucket_index(bx, by))
        {
            m_buckets[by * m_grid_width + bx].push_back(collider);
        }
    }

    void CollisionHandler::remove_bucket(Collider *collider, const size_t bx, const size_t by)
    {
        if (!valid_bucket_index(bx, by)) return;

        std::vector<Collider*> *bucket = &m_buckets[by * m_grid_width + bx];

        const size_t bsize = bucket->size();
        if (bsize > 1)
        {
            // Replace collider to be removed with collider in the back
            for (size_t i = 0; i < bsize - 1; i++)
            {
                if ((*bucket)[i] == collider)
                {
                    Collider* back = bucket->back();
                    (*bucket)[i] = back;
                    break;
                }
            }
        }

        bucket->pop_back();
    }

    void CollisionHandler::remove(Collider *collider)
    {
        if (collider->m_in_bucket)
        {
            const Recti& prev_buc_box = collider->m_bucket_box;
            glm::ivec2 prev_bl = prev_buc_box.bl;
            glm::ivec2 prev_tr = prev_buc_box.tr;

            for (int y = prev_bl.y; y <= prev_tr.y; y++)
            {
                for (int x = prev_bl.x; x <= prev_tr.x; x++)
                {
                    remove_bucket(collider, x, y);
                }
            }
        }
    }

    void CollisionHandler::update_all_buckets()
    {
        auto mnode = m_scene->first<Mover>();
        while (mnode)
        {
            Collider *col = mnode->data->collider;
            update_buckets(col);
            mnode = mnode->next;
        }
    }

    Recti CollisionHandler::bucket_box(const Rectf& bbox)
    {
        const glm::ivec2 bot_index = bucket_index(bbox.bl);
        const glm::ivec2 top_index = bucket_index(bbox.tr);

        return Recti(bot_index, top_index);
    }

    glm::ivec2 CollisionHandler::bucket_index(const glm::vec2& pos)
    {
        return glm::ivec2(((int)pos.x) >> 4, ((int)pos.y) >> 4);
    }

    bool CollisionHandler::valid_bucket_index(const size_t bx, const size_t by)
    {
        return bx >= 0 && bx < m_grid_width && 
            by >= 0 && by < m_grid_height;
    }

    void CollisionHandler::update()
    {
        std::unordered_map<size_t, bool> collided_map;
        update_all_buckets();

        for (size_t i = 0; i < collision_iterations; i++)
        {
            auto mnode = m_scene->first<Mover>();
            while (mnode)
            {
                auto mover = mnode->data;
                auto col = mover->collider;

                const Recti& buc_box = col->m_bucket_box;
                glm::ivec2 bl = buc_box.bl;
                glm::ivec2 tr = buc_box.tr;

                const size_t hash = std::hash<Collider*>{}(col);
                for (int y = bl.y; y <= tr.y; y++)
                {
                    for (int x = bl.x; x <= tr.x; x++)
                    {
                        if (!valid_bucket_index(x, y)) continue;

                        std::vector<Collider*> *bucket = &m_buckets[y * m_grid_width + x];

                        for (Collider *ocol : *bucket)
                        {
                            if (col != ocol && (mover->collides_with & ocol->mask))
                            {
                                // Make sure that same collision does not trigger multiple times
                                const size_t ohash = std::hash<Collider*>{}(ocol);
                                const size_t comb_hash = Calc::hash_combine(hash, ohash);
                                const bool has_collided = collided_map.find(comb_hash) 
                                    != collided_map.end();

                                if (!has_collided)
                                {
                                    const glm::vec2 push = col->push_out(*ocol);
                                    if (push != glm::vec2())
                                    {
                                        collided_map[comb_hash] = true;
                                        glm::vec2 push_norm = Calc::normalize(push);

                                        auto omover = ocol->get<Mover>();
                                        if (omover)
                                        {
                                            const glm::vec2 vel_diff = mover->vel - omover->vel;
                                            const float p = glm::dot(push_norm, vel_diff);

                                            //  Collision response
                                            const bool needs_push1 = !mover->on_hit || 
                                                !mover->on_hit(mover, ocol, push_norm);

                                            const bool needs_push2 = 
                                                !(omover->collides_with & col->mask) ||
                                                        !omover->on_hit || 
                                                        !omover->on_hit(omover, col, -push_norm);

                                            const float push_div = (float)(needs_push1 + 
                                                    needs_push2);

                                            if (needs_push1)
                                            {
                                                col->entity()->pos += push / push_div;
                                                mover->vel -= push_norm * p * collision_elasticity;
                                                col->invalidate_cache();
                                            }

                                            if (needs_push2)
                                            {
                                                ocol->entity()->pos -= push / push_div;
                                                omover->vel += push_norm * p * collision_elasticity;
                                                ocol->invalidate_cache();
                                            }
                                        }
                                        else
                                        {
                                            if (!mover->on_hit || 
                                                    !mover->on_hit(mover, ocol, push_norm))
                                            {
                                                col->entity()->pos += push;
                                                const float p = glm::dot(push_norm, mover->vel);
                                                mover->vel -= push_norm * p * collision_elasticity;
                                                col->invalidate_cache();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                mnode = mnode->next;
            }
        }
    }

    Collider *CollisionHandler::check(Collider *collider, const uint32_t mask)
    {
        if (!collider->m_in_bucket) return nullptr;

        const Recti& buc_box = collider->m_bucket_box;
        glm::ivec2 bl = buc_box.bl;
        glm::ivec2 tr = buc_box.tr;

        for (int y = bl.y; y <= tr.y; y++)
        {
            for (int x = bl.x; x <= tr.x; x++)
            {
                if (!valid_bucket_index(x, y)) continue;

                std::vector<Collider*> *bucket = &m_buckets[y * m_grid_width + x];
                for (Collider *other : *bucket)
                {
                    if (collider != other && (mask & other->mask))
                    {
                        if (collider->overlaps(*other))
                        {
                            return other;
                        }
                    }
                }
            }
        }

        return nullptr;
    }

    void CollisionHandler::check_all(Collider *collider, const uint32_t mask, 
            std::vector<Collider*> *out)
    {
        if (!collider->m_in_bucket) return;

        const Recti& buc_box = collider->m_bucket_box;
        glm::ivec2 bl = buc_box.bl;
        glm::ivec2 tr = buc_box.tr;

        for (int y = bl.y; y <= tr.y; y++)
        {
            for (int x = bl.x; x <= tr.x; x++)
            {
                if (!valid_bucket_index(x, y)) continue;

                std::vector<Collider*> *bucket = &m_buckets[y * m_grid_width + x];
                for (Collider *other : *bucket)
                {
                    if (collider != other && (mask & other->mask))
                    {
                        if (collider->overlaps(*other))
                        {
                            out->push_back(other);
                        }
                    }
                }
            }
        }
    }
}

#include "collisionhandler.h"
#include "collider.h"
#include "mover.h"
#include "ecs.h"
#include "tilemap.h"
#include <memory>

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

        if (collider->m_in_bucket)
        {
            glm::ivec2 prev_bot_left = prev_buc_box.bot_left();
            glm::ivec2 prev_top_right = prev_buc_box.top_right();

            for (int y = prev_bot_left.y; y <= prev_top_right.y; y++)
            {
                for (int x = prev_bot_left.x; x <= prev_top_right.x; x++)
                {
                    if (!buc_box.contains(glm::ivec2(x, y)))
                    {
                        remove_bucket(collider, x, y);
                    }
                }
            }
        }

        glm::ivec2 bot_left = buc_box.bot_left();
        glm::ivec2 top_right = buc_box.top_right();

        for (int y = bot_left.y; y <= top_right.y; y++)
        {
            for (int x = bot_left.x; x <= top_right.x; x++)
            {
                if (collider->m_in_bucket && !prev_buc_box.contains(glm::ivec2(x, y)))
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
        m_buckets[by * m_grid_width + bx].push_back(collider);
    }

    void CollisionHandler::remove_bucket(Collider *collider, const size_t bx, const size_t by)
    {
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
            glm::ivec2 prev_bot_left = prev_buc_box.bot_left();
            glm::ivec2 prev_top_right = prev_buc_box.top_right();

            for (int y = prev_bot_left.y; y <= prev_top_right.y; y++)
            {
                for (int x = prev_bot_left.x; x <= prev_top_right.x; x++)
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
        const glm::ivec2 bot_index = bucket_index(bbox.bot_left());
        const glm::ivec2 top_index = bucket_index(bbox.top_right());

        return Recti(bot_index, top_index - bot_index);
    }

    glm::ivec2 CollisionHandler::bucket_index(const glm::vec2& pos)
    {
        return glm::ivec2(((int)pos.x) >> 4, ((int)pos.y) >> 4);
    }

    void CollisionHandler::update()
    {
        update_all_buckets();
        auto mnode = m_scene->first<Mover>();
        while (mnode)
        {
            auto mover = mnode->data;
            auto col = mover->collider;

            const Recti& buc_box = col->m_bucket_box;
            glm::ivec2 bot_left = buc_box.bot_left();
            glm::ivec2 top_right = buc_box.top_right();

            for (int y = bot_left.y; y <= top_right.y; y++)
            {
                for (int x = bot_left.x; x <= top_right.x; x++)
                {
                    std::vector<Collider*>* bucket = &m_buckets[y * m_grid_width + x];

                    for (Collider *ocol : *bucket)
                    {
                        if (col != ocol && (mover->stop_mask & ocol->mask))
                        {
                            const glm::vec2 push = col->push_out(*ocol);
                            if (push != glm::vec2())
                            {
                                col->entity()->pos += push;
                                col->invalidate_cache();
                            }
                        }
                    }
                }
            }

            // auto onode = m_scene->first<Collider>();
            //
            // while (onode)
            // {
            //     auto ocol = onode->data;
            //     if (col != ocol && (mover->stop_mask & ocol->mask))
            //     {
            //         const glm::vec2 push = col->push_out(*ocol);
            //         if (push != glm::vec2())
            //         {
            //             col->entity()->pos += push;
            //             col->invalidate_cache();
            //         }
            //     }
            //
            //     onode = onode->next;
            // }

            mnode = mnode->next;
        }
    }
}

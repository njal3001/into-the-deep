#pragma once
#include "ecs.h"
#include "../maths/shapes.h"

namespace Uboat
{
    struct Mask
    {
        static constexpr uint32_t None = 0;
        static constexpr uint32_t Solid = 1;
        static constexpr uint32_t Player = 1 << 1;
        static constexpr uint32_t Enemy = 1 << 2;
    };

    class Collider : public Component
    {
        friend class CollisionHandler;

    public:
        struct Axes
        {
            glm::vec2 ax1;
            glm::vec2 ax2;
        };

        struct Projection
        {
            float start;
            float end;
        };

    public:
        Rectf bounds;
        float rotation;
        bool dynamic;
        uint32_t mask;

    private:
        Quadf m_quad;
        Axes m_axes;
        Rectf m_bbox;
        Recti m_bucket_box;
        bool m_in_bucket;

        int m_timestamp;

    public:
        Collider(const Rectf &bounds, const float rotation = 0.0f);

        const Quadf &quad();
        const Axes &axes();
        const Rectf &bbox();

        // Assumes that the collider is refreshed
        Projection project(const glm::vec2 &axis) const;

        void invalidate_cache();

        bool overlaps(Collider &other);
        glm::vec2 push_out(Collider &other);
        float distance(Collider &other);

        Collider *check(const uint32_t mask);
        void check_all(const uint32_t mask, std::vector<Collider*> *out);

        void render(Renderer *renderer) override;

    protected:
        void awake() override;
        void on_removed() override;

    private:
        void refresh();
        void recalculate();
    };
}

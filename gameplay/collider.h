#pragma once
#include "ecs.h"
#include "../maths/shapes.h"

namespace Uboat
{
    struct Mask
    {
        static constexpr uint32_t None = 0;
        static constexpr uint32_t Solid = 1;
    };

    class Collider : public Component
    {
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

    public:
        Collider(const Rectf& bounds, const bool dynamic = true, const float rotation = 0.0f);

        void awake() override;

        const Quadf& get();
        const Axes& axes();

        // Assumes that the collider is refreshed
        Projection project(const glm::vec2& axis) const;

        void invalidate_cache();

        bool overlaps(Collider& other);
        glm::vec2 push_out(Collider& other);

        void render(Renderer *renderer) override;

    private:
        Quadf m_cached;
        Axes m_axes;

        int m_timestamp;

    private:
        void refresh();
        void recalculate();
    };
}

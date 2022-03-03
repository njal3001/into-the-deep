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
        Rectf bounds;
        float rotation;
        bool dynamic;

        Collider(const Rectf& bounds, const bool dynamic, const float rotation = 0);

        Quadf get();
        void invalidate_cache();

    private:
        Quadf m_cached;
        uint64_t m_timestamp;

        void recalculate();
    };
}

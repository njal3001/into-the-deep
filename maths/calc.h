#pragma once
#include <glm/detail/qualifier.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include <stdio.h>

namespace Uboat
{
    namespace Calc
    {
        template <class T>
        glm::tvec2<T> normalize(const glm::tvec2<T>& vec)
        {
            const auto zero = glm::tvec2<T>();
            if (vec == zero)
            {
                return zero;
            }

            return glm::normalize(vec);
        }

        template <class T>
        glm::tvec2<T> approach(const glm::tvec2<T>& val, const glm::tvec2<T>& target, 
                const T amount)
        {
            if (glm::distance(val, target) <= amount)
            {
                return target;
            }

            const glm::tvec2<T> dir = Calc::normalize(target - val);

            return val + dir * amount;
        }

        template<class T>
        T approach(const T val, const T target, const T amount)
        {
            if (val + amount < target)
            {
                return std::min(val + amount, target);
            }

            return std::max(val - amount, target);
        }

        template<class T>
        T sgn(const T val)
        {
            return (T(0) < val) - (T(0) > val);
        }

        size_t hash_combine(size_t lhs, size_t rhs);

        float shortest_rotation_approach(const float val, const float target, const float period_multiplier);
    }
}

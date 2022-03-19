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

        size_t hash_combine(size_t lhs, size_t rhs);
    }
}

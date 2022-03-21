#pragma once
#include <glm/detail/qualifier.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include <stdio.h>

namespace Uboat
{
    namespace Calc
    {
        static constexpr glm::vec2 right = glm::vec2(1.0f, 0.0f);

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

        template<class T>
        glm::tvec2<T> rotate(const glm::tvec2<T> &point, const T amount, const glm::tvec2<T> &pivot)
        {
            const glm::tvec3<T> pivot3 = glm::tvec3<T>(pivot, 0);
            const glm::tmat4x4<T> rotate = glm::translate(
                    glm::rotate(glm::translate(glm::tmat4x4<T>(1), pivot3),
                        amount, glm::tvec3<T>(0, 0, 1)), -pivot3);

            return glm::tvec2<T>(rotate * glm::tvec4<T>(point, 0, 1));
        }

        size_t hash_combine(size_t lhs, size_t rhs);

        float shortest_rotation_approach(const float val, const float target, const float period_multiplier);
    }
}

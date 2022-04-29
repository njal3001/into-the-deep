#pragma once
#include <stdio.h>
#include <glm/detail/qualifier.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

namespace ITD {

namespace Calc {

    const float TAU = 2.0f * M_PI;

    constexpr glm::vec2 right = glm::vec2(1.0f, 0.0f);
    constexpr glm::vec2 left = glm::vec2(-1.0f, 0.0f);
    constexpr glm::vec2 up = glm::vec2(0.0f, 1.0f);
    constexpr glm::vec2 down = glm::vec2(0.0f, -1.0f);

    template <class T>
    glm::tvec2<T> normalize(const glm::tvec2<T> &vec)
    {
        const auto zero = glm::tvec2<T>();
        if (vec == zero)
        {
            return zero;
        }

        return glm::normalize(vec);
    }

    template <class T>
    glm::tvec2<T> approach(const glm::tvec2<T> &val,
                           const glm::tvec2<T> &target, T amount)
    {
        if (glm::distance(val, target) <= amount)
        {
            return target;
        }

        glm::tvec2<T> dir = Calc::normalize(target - val);

        return val + dir * amount;
    }

    template <class T>
    T approach(T val, T target, T amount)
    {
        if (val + amount < target)
        {
            return std::min(val + amount, target);
        }

        return std::max(val - amount, target);
    }

    template <class T>
    T sgn(T val)
    {
        return (T(0) < val) - (T(0) > val);
    }

    template <class T>
    glm::tmat4x4<T> rotate(T amount, const glm::tvec2<T> &pivot, const glm::tvec3<T> &axis = glm::tvec3<T>(0, 0, 1))
    {
        glm::tvec3<T> pivot3 = glm::tvec3<T>(pivot, 0);

        glm::tmat4x4<T> result = glm::translate(
            glm::rotate(glm::translate(glm::tmat4x4<T>(1), pivot3), amount,
                        axis),
            -pivot3);

        return result;
    }

    template <class T>
    glm::tmat4x4<T> scale(const glm::tvec2<T> &value,
                          const glm::tvec2<T> &reference)
    {
        glm::tvec3<T> ref3 = glm::tvec3<T>(reference, 0);
        glm::tvec3<T> val3 = glm::tvec3<T>(value, 1);

        glm::tmat4x4<T> result = glm::translate(
            glm::scale(glm::translate(glm::tmat4x4<T>(1), ref3), val3), -ref3);

        return result;
    }

    size_t hash_combine(size_t lhs, size_t rhs);

    float shortest_rotation_approach(float val, float target, float amount);

}  // namespace Calc

}  // namespace ITD

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
    glm::tvec2<T> normalize(const glm::tvec2<T> &vec);

    template <class T>
    glm::tvec2<T> approach(const glm::tvec2<T> &val,
                           const glm::tvec2<T> &target, T amount);

    template <class T>
    T approach(T val, T target, T amount);

    template <class T>
    T sgn(T val);

    template <class T>
    glm::tmat4x4<T> rotate(T amount, const glm::tvec2<T> &pivot,
                           const glm::tvec3<T> &axis = glm::tvec3<T>(0, 0, 1));

    template <class T>
    glm::tmat4x4<T> rotate(T amount, const glm::tvec3<T> &pivot,
                           const glm::tvec3<T> &axis = glm::tvec3<T>(0, 0, 1));

    template <class T>
    glm::tmat4x4<T> scale(const glm::tvec2<T> &value,
                          const glm::tvec2<T> &reference);

    template <class T>
    glm::tmat4x4<T> scale(const glm::tvec3<T> &value,
                          const glm::tvec3<T> &reference);

    size_t hash_combine(size_t lhs, size_t rhs);

    float shortest_rotation_approach(float val, float target, float amount);

    float randf(float lower, float upper);

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
    glm::tmat4x4<T> rotate(T amount, const glm::tvec2<T> &pivot,
                           const glm::tvec3<T> &axis)
    {
        glm::tvec3<T> pivot3 = glm::tvec3<T>(pivot, 0);

        return Calc::rotate(amount, pivot3, axis);
    }

    template <class T>
    glm::tmat4x4<T> rotate(T amount, const glm::tvec3<T> &pivot,
                           const glm::tvec3<T> &axis)
    {
        glm::tmat4x4<T> result = glm::translate(
            glm::rotate(glm::translate(glm::tmat4x4<T>(1), pivot), amount,
                        axis),
            -pivot);

        return result;
    }

    template <class T>
    glm::tmat4x4<T> scale(const glm::tvec2<T> &value,
                          const glm::tvec2<T> &reference)
    {
        glm::tvec3<T> val3 = glm::tvec3<T>(value, 1);
        glm::tvec3<T> ref3 = glm::tvec3<T>(reference, 0);

        return Calc::scale(val3, ref3);
    }

    template <class T>
    glm::tmat4x4<T> scale(const glm::tvec3<T> &value,
                          const glm::tvec3<T> &reference)
    {
        glm::tmat4x4<T> result = glm::translate(
            glm::scale(glm::translate(glm::tmat4x4<T>(1), reference), value),
            -reference);

        return result;
    }
}  // namespace Calc

}  // namespace ITD

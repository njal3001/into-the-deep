#pragma once
#include <glm/detail/qualifier.hpp>
#include <glm/geometric.hpp>

namespace Uboat
{
    namespace Calc
    {
        template <class T>
        glm::tvec2<T> normalize(glm::tvec2<T> vec)
        {
            auto zero = glm::tvec2<T>();
            if (vec == zero)
            {
                return zero;
            }

            return glm::normalize(vec);
        }
    }
}

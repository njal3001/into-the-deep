#include "calc.h"

namespace Uboat
{
    //https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
    size_t Calc::hash_combine(size_t lhs, size_t rhs)
    {
      lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
      return lhs;
    }

    float Calc::shortest_rotation_approach(const float val, const float target, const float period_multiplier)
    {
        // Find shortest rotation direction (clockwise or counter clockwise)
        const float a = target - val;
        const float b = target - val + 2 * M_PI;
        const float c = target - val - 2 * M_PI;

        float min = std::abs(a) < std::abs(b) ? a : b;
        min = std::abs(min) < std::abs(c) ? min : c;
        const float shortest_target_rotation = val + min;

        return fmod(approach(val, shortest_target_rotation, (float)(2.0f * M_PI * period_multiplier)) + 2.0f * M_PI, 2.0f * M_PI);
    }
}

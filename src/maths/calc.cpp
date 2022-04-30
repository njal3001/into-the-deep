#include "calc.h"
#include <random>

namespace ITD {

// https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
size_t Calc::hash_combine(size_t lhs, size_t rhs)
{
    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

float Calc::shortest_rotation_approach(float val, float target, float amount)
{
    // Find shortest rotation direction (clockwise or counter clockwise)
    float a = target - val;
    float b = target - val + TAU;
    float c = target - val - TAU;

    float min = std::abs(a) < std::abs(b) ? a : b;
    min = std::abs(min) < std::abs(c) ? min : c;
    float shortest_target_rotation = val + min;

    return fmod(approach(val, shortest_target_rotation, amount), TAU);
}

float Calc::randf(float lower, float upper)
{
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(lower, upper);

    return distribution(generator);
}

}  // namespace ITD

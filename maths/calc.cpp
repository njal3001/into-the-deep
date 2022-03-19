#include "calc.h"

namespace Uboat
{
    //https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
    size_t Calc::hash_combine(size_t lhs, size_t rhs)
    {
      lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
      return lhs;
    }
}

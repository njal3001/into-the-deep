#include "color.h"

namespace ITD {

Color::Color()
    : r(0)
    , g(0)
    , b(0)
    , a(0)
{
}

Color::Color(const int rgb)
    : r((uint8_t)((rgb & 0xFF0000) >> 16))
    , g((uint8_t)((rgb & 0x00FF00) >> 8))
    , b((uint8_t)(rgb & 0x0000FF))
    , a(255)
{
}

Color::Color(int rgb, float alpha)
    : r((uint8_t)((rgb & 0xFF0000) >> 16))
    , g((uint8_t)((rgb & 0x00FF00) >> 8))
    , b((uint8_t)(rgb & 0x0000FF))
    , a((uint8_t)(alpha * 255))
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b)
    : r(r)
    , g(g)
    , b(b)
    , a(255)
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{
}

uint32_t Color::rgb()
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b);
}

const Color Color::white = Color(255, 255, 255);
const Color Color::black = Color(0, 0, 0);
const Color Color::red = Color(255, 0, 0);
const Color Color::green = Color(0, 255, 0);
const Color Color::blue = Color(0, 0, 255);

}  // namespace ITD

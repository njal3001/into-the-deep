#pragma once
#include <stdint.h>

namespace ITD {

struct Color {
    uint8_t r, g, b, a;

    Color();

    Color(int rgb);
    Color(int rgb, float alpha);

    Color(uint8_t r, uint8_t g, uint8_t b);
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    uint32_t rgb();

    static const Color white;
    static const Color black;
    static const Color red;
    static const Color green;
    static const Color blue;
};

}  // namespace ITD

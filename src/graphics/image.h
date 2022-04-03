#pragma once
#include <string>
#include "color.h"

namespace ITD
{
    class Image
    {
    private:
        Color* m_pixels;
        size_t m_width;
        size_t m_height;

    public:
        Image(const std::string &filename);
        Image();
        ~Image();

        void load(const std::string &filename);

        Image(const Image& other) = delete;
        Image& operator=(const Image& other) = delete;

        const Color* pixels() const;
        size_t width() const;
        size_t height() const;
    };
}

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"
#include "image.h"

namespace ITD
{

    Image::Image(const std::string& filename)
    {
        int x, y, comp;
        unsigned char* data = stbi_load(filename.c_str(), &x, &y, &comp, 0);

        assert(data);

        m_pixels = (Color*)data;
        m_width = (size_t)x;
        m_height = (size_t)y;
    }

    Image::~Image()
    {
        stbi_image_free(m_pixels);
    }

    const Color* Image::pixels() const
    {
        return m_pixels;
    }

    size_t Image::width() const
    {
        return m_width;
    }

    size_t Image::height() const
    {
        return m_height;
    }
}

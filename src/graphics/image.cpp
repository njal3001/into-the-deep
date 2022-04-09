#define STB_IMAGE_IMPLEMENTATION
#include "image.h"

#include "../third_party/stb_image.h"

namespace ITD {

Image::Image(const std::string &filename)
    : Image()
{
    load(filename);
}

Image::Image()
    : m_pixels(nullptr)
    , m_width(0)
    , m_height(0)
{
}

Image::~Image()
{
    stbi_image_free(m_pixels);
}

void Image::load(const std::string &filename)
{
    assert(!m_pixels);

    int x, y, comp;
    unsigned char *data = stbi_load(filename.c_str(), &x, &y, &comp, 0);

    assert(data);

    m_pixels = (Color *)data;
    m_width = (size_t)x;
    m_height = (size_t)y;
}

const Color *Image::pixels() const
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

}  // namespace ITD

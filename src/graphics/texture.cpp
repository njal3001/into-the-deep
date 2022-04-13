#include "texture.h"
#include "../debug.h"

namespace ITD {

Texture::Texture(size_t width, size_t height, const unsigned char *data)
    : Texture()
{
    load(width, height, data);
}

Texture::Texture(const Image &image)
    : Texture()
{
    load(image);
}

Texture::Texture()
    : m_id(0)
    , m_width(0)
    , m_height(0)
{
}

void Texture::load(size_t width, size_t height, const unsigned char *data)
{
    ITD_ASSERT(m_id == 0, "Can not load texture multiple times");
    ITD_ASSERT(width > 0 && height > 0,
               "Texture width and height must be greater than 0");

    m_width = width;
    m_height = height;

    glGenTextures(1, &m_id);

    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::load(const Image &image)
{
    load(image.width(), image.height(), (unsigned char *)image.pixels());
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

GLuint Texture::id() const
{
    return m_id;
}

size_t Texture::width() const
{
    return m_width;
}

size_t Texture::height() const
{
    return m_height;
}

}  // namespace ITD

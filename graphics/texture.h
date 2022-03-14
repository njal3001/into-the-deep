#pragma once
#include <GL/glew.h>
#include "image.h"

namespace Uboat
{
    class Texture
    {
    private:
        GLuint m_id;
        GLsizei m_width, m_height;

    public:
        Texture(const size_t width, const size_t height, const unsigned char* data);
        Texture(const Image& image);
        Texture();
        ~Texture();

        Texture(const Texture& other) = delete;
        Texture& operator=(const Texture& other) = delete;

        void load(const size_t width, const size_t height, const unsigned char* data);
        void load(const Image& image);

        GLuint id() const;
        size_t width() const;
        size_t height() const;
    };}

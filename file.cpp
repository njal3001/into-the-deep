#include "file.h"
#include <SDL2/SDL.h>

namespace ITD
{
    File::File(const std::string& path)
        : path(path), size(0), data(nullptr)
    {
        SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "rb");
        if (!rw)
        {
            return;
        }

        Sint64 size = SDL_RWsize(rw);
        char *buffer = new char[size + 1];

        Sint64 read = SDL_RWread(rw, buffer, sizeof(char), size);

        SDL_RWclose(rw);
        if (read != size)
        {
            delete[] buffer;
            return;
        }

        buffer[size] = '\0';

        size = (size_t)size;
        data = buffer;
    }

    File::~File()
    {
        if (data)
            delete[] data;
    }
}

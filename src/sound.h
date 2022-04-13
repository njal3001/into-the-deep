#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>

namespace ITD {
class Sound
{
private:
    Mix_Chunk *m_data;

public:
    Sound();
    Sound(const std::string &path);
    ~Sound();

    void play(int channel = -1, bool loop = false);
};
}  // namespace ITD

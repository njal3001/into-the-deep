#include "sound.h"
#include "debug.h"
#include "platform.h"

namespace ITD {

Sound::Sound()
    : m_data(nullptr)
{
}
Sound::Sound(const std::string &path)
    : m_data(nullptr)
{
    m_data = Mix_LoadWAV(path.c_str());
    ITD_ASSERT(m_data, "Could not load sound data");
}

Sound::~Sound()
{
    if (m_data)
    {
        Mix_FreeChunk(m_data);
    }
}

void Sound::play(int channel, bool loop)
{
    // TODO: Should propably have some audio player that plays all sounds
    if (m_data && !Platform::muted())
    {
        Mix_PlayChannel(channel, m_data, loop);
    }
}

}  // namespace ITD

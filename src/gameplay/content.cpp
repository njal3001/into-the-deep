#include "content.h"
#include <string>
#include <unordered_map>
#include "../platform.h"

namespace ITD {
namespace {
    std::unordered_map<std::string, Sound> g_sounds;
}

Sound *Content::find_sound(const std::string &name)
{
    if (g_sounds.find(name) == g_sounds.end())
    {
        // TODO: Compute directory at initialization
        std::string path = Platform::app_path() + "../res/" + name + ".wav";

        g_sounds.emplace(name, path);
    }

    return &g_sounds[name];
}

}  // namespace ITD

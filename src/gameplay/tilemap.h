#pragma once
#include "../graphics/renderer.h"
#include "../third_party/json.hpp"
#include "ecs.h"

namespace ITD {

class Tilemap
{
private:
    std::string m_name;
    int m_width, m_height;
    Texture m_texture;
    nlohmann::json m_data;
    Image m_igrid;

public:
    Tilemap(const std::string &name);

    void fill_scene(Scene *scene);
    void render(Renderer *renderer);

    size_t width() const;
    size_t height() const;
};

}  // namespace ITD

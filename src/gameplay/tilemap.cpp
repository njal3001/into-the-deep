#include "tilemap.h"
#include <iostream>
#include "../file.h"
#include "../platform.h"
#include "chaser.h"
#include "factory.h"
#include "player.h"
#include "playerhud.h"

namespace ITD {

using namespace nlohmann;

#define WALL 0xFB0000

Tilemap::Tilemap(const std::string &name)
    : m_name(name)
    , m_width(0)
    , m_height(0)
{
    const std::string base_path =
        Platform::app_path() + "../res/map/simplified/" + m_name + "/";

    const std::string img_path = base_path + "_composite.png";
    const std::string igrid_path = base_path + "IntGrid-int.png";
    const std::string data_path = base_path + "data.json";

    Image img(img_path);
    m_texture.load(img);

    m_igrid.load(igrid_path);

    File data_file(data_path);
    m_data = json::parse(data_file.data);

    m_width = m_data["width"].get<int>() / 8;
    m_height = m_data["height"].get<int>() / 8;
}

void Tilemap::fill_scene(Scene *scene)
{
    // Tilemap
    const Color *pixels = m_igrid.pixels();
    for (int y = 0; y < m_igrid.height(); y++)
    {
        for (int x = 0; x < m_igrid.width(); x++)
        {
            Color col = pixels[x + y * m_igrid.width()];

            glm::vec2 pos(x * 8.0f, (m_height - 1 - y) * 8.0f);

            switch (col.rgb())
            {
                case WALL: {
                    bool redundant = true;
                    for (int oy = std::max(0, y - 1);
                         oy <= std::min(m_height - 1, y + 1); oy++)
                    {
                        for (int ox = std::max(0, x - 1);
                             ox <= std::min(m_width - 1, x + 1); ox++)
                        {
                            Color neighbor = pixels[ox + oy * m_igrid.width()];
                            if (neighbor.rgb() != WALL)
                            {
                                redundant = false;
                            }
                        }
                    }

                    if (!redundant)
                    {
                        Factory::wall(scene, pos);
                    }

                    break;
                }
            }
        }
    }

    // Entities
    float pixel_height = m_height * 8.0f;
    auto entities = m_data["entities"];
    for (auto it = entities.begin(); it != entities.end(); it++)
    {
        const auto &key = it.key();
        const auto &val = it.value();

        for (auto ent_it = val.begin(); ent_it != val.end(); ent_it++)
        {
            const auto &ent_val = ent_it.value();
            int x = ent_val["x"].get<int>();
            int y = ent_val["y"].get<int>();

            glm::vec2 pos(x, pixel_height - y);

            if (key == "Player")
            {
                Player::create(scene, pos);
            }
            else if (key == "Chaser")
            {
                Chaser::create(scene, pos);
            }
        }
    }
}

void Tilemap::render(Renderer *renderer)
{
    renderer->tex(&m_texture, glm::vec2(), Color::white);
}

size_t Tilemap::width() const
{
    return m_width;
}

size_t Tilemap::height() const
{
    return m_height;
}

}  // namespace ITD

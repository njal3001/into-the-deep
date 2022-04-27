#include "tilemap.h"
#include <iostream>
#include "../file.h"
#include "../platform.h"
#include "chaser.h"
#include "factory.h"
#include "player.h"
#include "playerhud.h"
#include "wall.h"

namespace ITD {

using namespace nlohmann;

#define WALL 0xFB0000

Tilemap::Tilemap(const std::string &name)
    : m_name(name)
    , m_width(0)
    , m_height(0)
{
    std::string base_path =
        Platform::app_path() + "../res/map/simplified/" + m_name + "/";

    std::string img_path = base_path + "_composite.png";
    std::string igrid_path = base_path + "IntGrid-int.png";
    std::string data_path = base_path + "data.json";

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
                    uint8_t direction_mask = 0;

                    glm::ivec2 directions[4] = {
                        glm::ivec2(0, -1), glm::ivec2(1, 0), glm::ivec2(-1, 0),
                        glm::ivec2(0, 1)};

                    for (uint8_t i = 0; i < 4; i++)
                    {
                        glm::ivec2 direction = directions[i];
                        int ox = direction.x + x;
                        int oy = direction.y + y;

                        if (ox >= 0 && ox < m_width && oy >= 0 && oy < m_height)
                        {
                            Color neighbor = pixels[ox + oy * m_igrid.width()];
                            if (neighbor.rgb() != WALL)
                            {
                                redundant = false;
                                direction_mask |= (1 << i);
                            }
                        }
                    }

                    if (!redundant)
                    {
                        Wall::create(scene, pos, direction_mask);
                    }

                    break;
                }
            }
        }
    }

    // Entities
    float pheight = pixel_height();
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

            glm::vec2 pos(x, pheight - y);

            if (key == "Player")
            {
                Player::create(scene, pos);
                PlayerHUD::create(scene);
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
    // renderer->tex(&m_texture, glm::vec2(), Color::white);
}

size_t Tilemap::width() const
{
    return m_width;
}

size_t Tilemap::height() const
{
    return m_height;
}

float Tilemap::pixel_width() const
{
    return m_width * 8.0f;
}

float Tilemap::pixel_height() const
{
    return m_height * 8.0f;
}

}  // namespace ITD

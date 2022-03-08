#include "tilemap.h"
#include "../platform.h"
#include "factory.h"
#include "player.h"

namespace Uboat
{
    Tilemap::Tilemap(const std::string& name)
        : m_name(name), m_width(0), m_height(0), m_texture(nullptr)
    {
        using namespace rapidxml;

        // Load texture
        const std::string image_path = Platform::app_path() + "../res/" + name + ".png";
        Image img(image_path);

        m_texture = new Texture(img);

        // Load map
        const std::string map_path = Platform::app_path() + "../res/" + m_name + ".tmx";
        File file = Platform::read_file(map_path);

        xml_document<> doc;
        doc.parse<0>(file.data);

        xml_node<> *map_node = doc.first_node("map");

        m_width = (size_t)strtol(map_node->first_attribute("width")->value(),
                nullptr, 10);
        m_height = (size_t)strtol(map_node->first_attribute("height")->value(),
                nullptr, 10);

        // Tile layer
        xml_node<> *tiles_node = map_node->first_node("layer");
        create_layer(&m_tile_layer, tiles_node, m_width, m_height);

        // Object layer
        xml_node<> *objects_node = tiles_node->next_sibling();
        create_layer(&m_object_layer, objects_node, m_width, m_height);

        // TODO: This should be done in the struct
        delete[] file.data;
    }

    Tilemap::~Tilemap()
    {
        delete m_texture;
    }

    void Tilemap::fill_scene(Scene *scene)
    {
        constexpr size_t solid_id = 2;
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                const size_t tile_id = m_tile_layer[y * m_width + x];
                if (tile_id == solid_id)
                {
                    // Remove redundant solid tiles
                    bool redundant = true;
                    for (int ox = std::max(0, x - 1); ox <= std::min(m_width - 1, x + 1); ox++)
                    {
                        for (int oy = std::max(0, y - 1); oy <= std::min(m_height - 1, y + 1); oy++)
                        {
                            if (m_tile_layer[oy * m_width + ox] != solid_id)
                            {
                                redundant = false;
                            }
                        }
                    }

                    if (redundant)
                    {
                        // Skip to next tile
                        continue;
                    }
                }

                add_entity(tile_id, scene, x, y);
            }
        }

        // Add object entities
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                const size_t object_id = m_object_layer[y * m_width + x];
                add_entity(object_id, scene, x, y);
            }
        }
    }

    void Tilemap::create_layer(std::vector<size_t> *out, const rapidxml::xml_node<> *node, 
            const int width, const int height)
    {
        out->resize(width * height);
        char *data = node->first_node("data")->value();

        char *replace;
        for (int y = height - 1; y >= 0; y--)
        {
            for (int x = 0; x < width; x++)
            {
                replace = strchr(data, ',');
                if (replace)
                {
                    *replace = '\0';
                }

                const size_t id = strtol(data, nullptr, 10);
                (*out)[y * width + x] = id;
                data = replace + 1;
            }
        }
    }

    void Tilemap::add_entity(const size_t id, Scene *scene, const size_t x, const size_t y)
    {
        glm::vec2 pos(x * 8.0f, y * 8.0f);

        switch (id)
        {
            case 2:
            {
                Factory::wall(scene, pos);
                break;
            }
            case 3:
            {
                Player::create(scene, pos);
                break;
            }
        }
    }

    void Tilemap::render(Renderer *renderer)
    {
        renderer->tex(m_texture, glm::vec2(), Color::white);
    }
}

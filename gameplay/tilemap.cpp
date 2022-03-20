#include "tilemap.h"
#include "../platform.h"
#include "../file.h"
#include "factory.h"
#include "player.h"
#include "chaser.h"
#include "playerhud.h"

namespace Uboat
{
    using namespace rapidxml;

    Tilemap::Tilemap(const std::string& name)
        : m_name(name), m_width(0), m_height(0)
    {

        // Load map
        // TODO: Use tileset for mask
        const std::string map_path = Platform::app_path() + "../res/" + 
            m_name + ".tmx";
        File file(map_path);

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

        // Build texture
        xml_node<> *tile_set_node = map_node->first_node("tileset");
        build_texture(tile_set_node);
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
                    for (int oy = std::max(0, y - 1); oy <= std::min(m_height - 1, y + 1); oy++)
                    {
                        for (int ox = std::max(0, x - 1); ox <= std::min(m_width - 1, x + 1); ox++)
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

    void Tilemap::create_layer(std::vector<size_t> *out, const xml_node<> *node, 
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
                PlayerHUD::create(scene);
                break;
            }
            case 4:
            {
                Chaser::create(scene, pos);
                break;
            }
        }
    }

    void Tilemap::build_texture(const xml_node<> *tile_set_node)
    {
        // Load tileset image
        std::string tile_set_path = Platform::app_path() + "../res/" + 
            std::string(tile_set_node->first_attribute("source")->value());

        File file(tile_set_path);
        xml_document<> doc;
        doc.parse<0>(file.data);

        xml_node<> *root = doc.first_node("tileset");
        xml_node<> *img_node = root->first_node("image");

        const std::string image_path = Platform::app_path() + "../res/" +
            std::string(img_node->first_attribute("source")->value());

        Image img(image_path);

        // Generate texture by combining tileset and tilemap data
        const Color *img_data = img.pixels();

        const size_t tex_width = m_width * tile_size;
        const size_t tex_height = m_height * tile_size;
        Color tex_data[tex_width * tex_height];

        for (int y = m_height - 1; y >= 0; y--)
        {
            for (int x = 0; x < m_width; x++)
            {
                const int id = m_tile_layer[y * m_width + x] - 1;

                for (int oy = 0; oy < tile_size; oy++)
                {
                    for (int ox = 0; ox < tile_size; ox++)
                    {
                        const int px = x * tile_size + ox;
                        const int py = tex_height - 1 - (y * tile_size + oy);
                        const int tex_index = px + py * tex_width;

                        const int img_index = id * tile_size + oy * img.width() + ox;
                        tex_data[tex_index] = img_data[img_index];
                    }
                }
            }
        }

        m_texture.load(tex_width, tex_height, (unsigned char*)tex_data);
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
}

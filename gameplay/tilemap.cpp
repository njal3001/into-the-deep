#include "tilemap.h"
#include "../platform.h"
#include "rapidxml/rapidxml.hpp"
#include "factory.h"
#include "player.h"

namespace Uboat
{
    Tilemap::Tilemap(const std::string& name)
        : m_name(name), m_width(0), m_height(0), m_texture(nullptr)
    {
        const std::string image_path = Platform::app_path() + "../res/" + name + ".png";
        Image img(image_path);

        m_texture = new Texture(img);
    }

    Tilemap::~Tilemap()
    {
        delete m_texture;
    }

    void Tilemap::load(Scene *scene)
    {
        using namespace rapidxml;

        const std::string path = Platform::app_path() + "../res/" + m_name + ".tmx";
        File file = Platform::read_file(path);

        xml_document<> doc;
        doc.parse<0>(file.data);

        xml_node<> *map_node = doc.first_node("map");

        const int width = (size_t)strtol(map_node->first_attribute("width")->value(),
                nullptr, 10);
        const int height = (size_t)strtol(map_node->first_attribute("height")->value(),
                nullptr, 10);

        map.resize(width * height);

        // Background tiles
        xml_node<> *layer_node = map_node->first_node("layer");
        char *data = layer_node->first_node("data")->value();

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
                map[y * width + x] = id;
                data = replace + 1;
                add_entity(id, scene, x, y);
            }
        }

        // Object tiles
        layer_node = layer_node->next_sibling();
        data = layer_node->first_node("data")->value();

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
                data = replace + 1;
                add_entity(id, scene, x, y);
            }
        }

        delete[] file.data;
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

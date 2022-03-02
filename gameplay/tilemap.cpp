#include "tilemap.h"
#include "../platform.h"
#include "rapidxml/rapidxml.hpp"

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

        const size_t width = (size_t)strtol(map_node->first_attribute("width")->value(), nullptr, 10);
        const size_t height = (size_t)strtol(map_node->first_attribute("height")->value(), nullptr, 10);

        map.resize(width * height);

        // Background tiles
        xml_node<> *layer_node = map_node->first_node("layer");
        char *data = layer_node->first_node("data")->value();

        char *replace;
        for (size_t i = 0; i < width * height - 1; i++)
        {
            replace = strchr(data, ',');
            *replace = '\0';
            map[i] = (size_t)strtol(data, nullptr, 10);
            data = replace + 1;
        }

        map[width * height] = (size_t)strtol(data, nullptr, 10);

        // Object tiles
        layer_node = layer_node->next_sibling();
        data = layer_node->first_node("data")->value();

        for (size_t i = 0; i < width * height - 1; i++)
        {
            replace = strchr(data, ',');
            *replace = '\0';
            size_t val = (size_t)strtol(data, nullptr, 10);
            data = replace + 1;

            add_entity(val, scene);
        }

        add_entity((size_t)strtol(data, nullptr, 10), scene);

        delete[] file.data;
    }

    void Tilemap::add_entity(const size_t id, Scene *scene)
    {
        if (id == 0) return;
    }

    void Tilemap::render(Renderer *renderer)
    {
        renderer->tex(m_texture, glm::vec2(), Color::white);
    }
}

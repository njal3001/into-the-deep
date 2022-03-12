#pragma once
#include "../graphics/renderer.h"
#include "ecs.h"
#include "rapidxml/rapidxml.hpp"

namespace Uboat
{
    class Tilemap
    {
    private:
        std::string m_name;
        int m_width, m_height;
        Texture *m_texture;
        std::vector<size_t> m_tile_layer;
        std::vector<size_t> m_object_layer;

    public:
        Tilemap(const std::string& name);
        ~Tilemap();

        void fill_scene(Scene *scene);
        void render(Renderer *renderer);

        size_t width() const;
        size_t height() const;

    private:
        void create_layer(std::vector<size_t> *out, const rapidxml::xml_node<> *node, 
                const int width, const int height);
        void add_entity(const size_t id, Scene *scene, const size_t x, const size_t y);
    };
}

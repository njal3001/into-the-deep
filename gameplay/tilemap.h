#pragma once
#include "../graphics/renderer.h"
#include "ecs.h"

namespace Uboat
{
    class Tilemap
    {
    private:
        std::string m_name;
        size_t m_width, m_height;
        Texture *m_texture;
        std::vector<size_t> map;

    public:
        Tilemap(const std::string& name);
        ~Tilemap();

        void load(Scene *scene);
        void render(Renderer *renderer);

    private:
        void add_entity(const size_t id, Scene *scene, const size_t x, const size_t y);
    };
}

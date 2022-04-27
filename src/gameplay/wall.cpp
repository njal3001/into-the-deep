#include "wall.h"
#include "collider.h"

namespace ITD {

Wall::Wall(uint8_t directions)
    : m_directions(directions)
{
}

void Wall::render(Renderer *renderer)
{
    Collider *collider = get<Collider>();
    Quad quad = collider->quad();
    glm::vec2 center = quad.center();

    float line_thickness = 2.0f;
    if (m_directions & Direction::North)
    {
        glm::vec2 norm = Calc::normalize(center - (quad.b + quad.c) / 2.0f);
        renderer->line(quad.b, quad.c, line_thickness, norm, Color::white);
    }

    if (m_directions & Direction::East)
    {
        glm::vec2 norm = Calc::normalize(center - (quad.c + quad.d) / 2.0f);
        renderer->line(quad.c, quad.d, line_thickness, norm, Color::white);
    }

    if (m_directions & Direction::South)
    {
        glm::vec2 norm = Calc::normalize(center - (quad.a + quad.d) / 2.0f);
        renderer->line(quad.a, quad.d, line_thickness, norm, Color::white);
    }

    if (m_directions & Direction::West)
    {
        glm::vec2 norm = Calc::normalize(center - (quad.a + quad.b) / 2.0f);
        renderer->line(quad.a, quad.b, line_thickness, norm, Color::white);
    }
}

Entity *Wall::create(Scene *scene, const glm::vec2 &pos, uint8_t directions)
{
    Entity *e = scene->add_entity(pos);

    Wall *wall = new Wall(directions);
    e->add(wall);

    Collider *c = new Collider(
        Rectf(glm::vec2(0.0f, 0.0f), glm::vec2(8.0f, 8.0f)), 0.0f, false);
    c->mask = Mask::Solid;
    e->add(c);

    return e;
}

}  // namespace ITD

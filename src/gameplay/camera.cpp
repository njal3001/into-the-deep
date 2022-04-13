#include "camera.h"
#include "ecs.h"
#include "player.h"
#include "tilemap.h"
#include "../graphics/graphics.h"

namespace ITD {

Camera::Camera()
    : m_scene(nullptr)
    , m_pos(glm::vec2())
    , m_target(glm::vec2())
{
}

void Camera::init(const Scene *scene)
{
    ITD_ASSERT(!m_scene, "Can't initialize camera multiple times");
    m_scene = scene;
}

void Camera::set_pos(const glm::vec2 &pos)
{
    m_pos = to_screen_center(pos);
    m_target = m_pos;
}

void Camera::set_target(const glm::vec2 &target)
{
    m_target = to_screen_center(target);
}

// The camera position that will put the argument position
// in the center of the screen
glm::vec2 Camera::to_screen_center(const glm::vec2 &pos)
{
    glm::vec2 pixel_screen_size = Graphics::pixel_screen_size;
    const Tilemap *map = m_scene->map();

    glm::vec2 cam_pos = pos - pixel_screen_size / 2.0f;

    // TODO: Why minus one?
    cam_pos.x = std::clamp(cam_pos.x, 0.0f, map->pixel_width() - pixel_screen_size.x - 1.0f);
    cam_pos.y = std::clamp(cam_pos.y, 0.0f, map->pixel_height() - pixel_screen_size.y - 1.0f);

    return cam_pos;
}

void Camera::update(float elapsed)
{
    m_pos = Calc::approach(m_pos, m_target, speed * elapsed);
}

glm::mat4 Camera::get_matrix() const
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(-m_pos, 0.0f));
}

}  // namespace ITD

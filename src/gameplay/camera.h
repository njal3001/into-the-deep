#pragma once
#include <glm/glm.hpp>

namespace ITD {

class Scene;
class Tilemap;

class Camera
{
private:
    // TODO: Make this configurable?
    static constexpr float speed = 150.0f;

    const Scene *m_scene;
    glm::vec2 m_pos;
    glm::vec2 m_target;

public:
    Camera();

    void init(const Scene *scene);

    void set_pos(const glm::vec2 &pos);
    void set_target(const glm::vec2 &target);

    void update(float elapsed);

    glm::mat4 get_matrix() const;

private:
    glm::vec2 to_screen_center(const glm::vec2 &pos);

};
}  // namespace ITD

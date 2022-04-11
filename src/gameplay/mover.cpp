#include "mover.h"
#include <glm/gtx/vector_angle.hpp>
#include "collider.h"

namespace ITD {

Mover::Mover()
    : vel(glm::vec2())
    , facing(glm::vec2(1.0f, 0.0f))
    , target_speed(0.0f)
    , accel(0.0f)
    , approach_target(true)
{
}

void Mover::rotate_towards(const glm::vec2 &target_dir, float amount)
{
    float current_rotation =
        glm::orientedAngle(glm::vec2(facing.x, -facing.y), Calc::right);

    float target_rotation =
        glm::orientedAngle(glm::vec2(target_dir.x, -target_dir.y), Calc::right);

    float new_rotation = Calc::shortest_rotation_approach(
        current_rotation, target_rotation, amount);

    facing = glm::rotate(Calc::right, new_rotation);
}

void Mover::update(float elapsed)
{
    if (approach_target)
    {
        vel = Calc::approach(vel, facing * target_speed, accel * elapsed);
    }

    m_entity->translate(vel * elapsed);
}

}  // namespace ITD

#pragma once
#include <glm/glm.hpp>

namespace ITD {

class Player;

class PlayerInput
{
private:
    static constexpr float input_buffer_time = 0.2f;

    glm::vec2 m_move_dir;
    glm::vec2 m_shoot_dir;
    float m_shoot_buffer_timer;
    float m_dash_buffer_timer;

public:
    PlayerInput();

    void update(Player *player, float elapsed);

    glm::vec2 move_dir() const;
    glm::vec2 shoot_dir() const;

    bool shoot() const;
    bool dash() const;

    bool consume_dash();
    bool consume_shoot();
};

}  // namespace ITD

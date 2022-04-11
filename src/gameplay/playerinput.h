#include <glm/glm.hpp>

namespace ITD {

class PlayerInput
{
private:
    static constexpr float input_buffer_time = 0.2f;

    glm::vec2 m_dir;
    float m_shoot_buffer_timer;
    float m_dash_buffer_timer;

public:
    PlayerInput();

    void update(float elapsed);

    glm::vec2 dir() const;

    bool shoot() const;
    bool dash() const;

    bool consume_dash();
    bool consume_shoot();
};

}  // namespace ITD

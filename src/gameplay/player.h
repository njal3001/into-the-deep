#pragma once
#include "ecs.h"
#include "playerinput.h"

namespace ITD {

class Player : public Component
{
private:
    static constexpr float max_speed = 100.0f;
    static constexpr float accel = 150.0f;
    static constexpr float dash_speed = 250.0f;
    static constexpr float dash_time = 0.4f;
    static constexpr float dash_max_speed_time = 0.2f;
    static constexpr float dash_deaccel = 600.0f;
    static constexpr float dash_cooldown = 0.8f;
    static constexpr float shoot_knockback = 5.0f;
    static constexpr float rotation_multiplier = 0.90f;
    static constexpr int max_torpedo_ammo = 3;
    static constexpr float shoot_delay = 0.2f;
    static constexpr float reload_time = 1.5f;
    static constexpr float torpedo_start_speed = 20.0f;
    static constexpr float new_particle_time = 0.08f;

    float m_dash_timer;
    float m_dash_cooldown_timer;
    float m_shoot_cooldown_timer;
    int m_torpedo_ammo;
    float m_shoot_delay_timer;
    float m_reload_timer;
    float m_wing_rotation;
    float m_new_particle_timer;

    PlayerInput m_player_input;

public:
    Player();

    int torpedo_ammo() const;

    void update(float elapsed) override;
    void render(Renderer *renderer) override;

    static Entity *create(Scene *scene, const glm::vec2 &pos);

private:
    Trif create_wing(const glm::vec2 &line_start, const glm::vec2 &line_end,
                     float span) const;

    void render_wing(Renderer *renderer, const Trif &wing, float z, float depth, Color color) const;
};

}  // namespace ITD

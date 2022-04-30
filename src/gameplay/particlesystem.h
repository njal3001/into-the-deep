#pragma once
#include <glm/glm.hpp>
#include "../graphics/renderer.h"

namespace ITD {

struct Particle {
    glm::vec2 pos;
    glm::vec2 vel;
    float radius;
    float time_left;
};

class ParticleSystem
{
private:
    static const size_t max_particles = 1024;
    Particle m_particles[max_particles];
    size_t m_particle_count;

public:
    ParticleSystem();

    void add_particle(const glm::vec2 &pos, const glm::vec2 &vel, float radius,
                      float life_time);

    void update(float elapsed);
    void render(Renderer *renderer);

private:
    void remove_particle(size_t index);
};
}  // namespace ITD

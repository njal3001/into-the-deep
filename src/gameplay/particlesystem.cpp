#include "particlesystem.h"
#include "../debug.h"

namespace ITD {

ParticleSystem::ParticleSystem()
    : m_particle_count(0)
{
}

void ParticleSystem::add_particle(const glm::vec2 &pos, const glm::vec2 &vel,
                                  float radius, float life_time)
{
    ITD_ASSERT(m_particle_count < max_particles, "Max particles exceeded");

    Particle &p = m_particles[m_particle_count];
    p.pos = pos;
    p.vel = vel;
    p.radius = radius;
    p.time_left = life_time;

    m_particle_count++;
}

void ParticleSystem::remove_particle(size_t index)
{
    m_particle_count--;
    if (index != m_particle_count)
    {
        // Fill gap with last particle
        m_particles[index] = m_particles[m_particle_count];
    }
}

void ParticleSystem::update(float elapsed)
{
    for (size_t i = 0; i < m_particle_count; i++)
    {
        Particle &p = m_particles[i];
        p.pos += p.vel * elapsed;
        p.time_left -= elapsed;

        if (p.time_left <= 0.0f)
        {
            remove_particle(i);
        }
    }
}

void ParticleSystem::render(Renderer *renderer)
{
    for (size_t i = 0; i < m_particle_count; i++)
    {
        Particle &p = m_particles[i];
        renderer->circ(p.pos, p.radius, 16, Color::green);
    }
}

}  // namespace ITD

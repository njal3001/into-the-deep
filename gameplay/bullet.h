#include "ecs.h"

namespace Uboat
{
    class Bullet : public Component
    {
    private:
        static constexpr float life_time = 0.35f;
        float m_life_timer;

    public:
        Bullet();

        void awake() override;
        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2 &pos, const glm::vec2& vel);

    private:
        void on_hit(Collider *other, const glm::vec2 &dir);
    };
}

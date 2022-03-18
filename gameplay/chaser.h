#include "ecs.h"

namespace Uboat
{
    class Chaser : public Component
    {
    public:
        void update(const float elapsed) override;
        void render(Renderer *renderer) override;

        static Entity* create(Scene *scene, const glm::vec2& pos);
    };
}

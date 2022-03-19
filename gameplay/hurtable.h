#include "ecs.h"

namespace Uboat
{
    class Hurtable : public Component
    {
    public:
        int health;
        
        void hurt();
    };
}

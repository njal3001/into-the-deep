#pragma once

namespace Uboat
{
    class Scene;

    class CollisionHandler
    {
    private:
        Scene *m_scene;

    public:
        CollisionHandler();

        void init(Scene *scene);
        void update();
    };
}

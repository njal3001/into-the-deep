#include <glm/gtc/matrix_transform.hpp>
#include "gameplay/tilemap.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/renderer.h"
#include "input.h"
#include "maths/shapes.h"
#include "platform.h"
#include "gameplay/collider.h"
#include "gameplay/player.h"
#include "gameplay/mover.h"

int main()
{
    using namespace Uboat;

    Platform::init();
    Renderer renderer;

    Scene::register_component<Collider>();
    Scene::register_component<Player>(Property::Updatable | Property::Renderable);
    Scene::register_component<Mover>(Property::Updatable);

    Tilemap map("tilemap");
    Scene scene(&map);

    glm::mat4 matrix = glm::ortho(0.0f, 320.0f, 0.0f, 180.0f);

    uint64_t prev_ticks = Platform::ticks();
    float elapsed = 0.0f; // In seconds

    while (Platform::update())
    {
        // Calculate elapsed time
        const uint64_t current_ticks = Platform::ticks();
        const uint64_t tick_diff = current_ticks - prev_ticks;
        elapsed = (tick_diff) / (float)Platform::ticks_per_sec;
        prev_ticks = current_ticks;

        // Update
        scene.update(elapsed);

        // Render
        Graphics::clear(Color::blue);

        renderer.begin();
        scene.render(&renderer);
        renderer.end();

        renderer.render(matrix);

        Graphics::present();
    }

    Platform::shutdown();
}

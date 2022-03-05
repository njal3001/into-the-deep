#include "gameplay/player.h"
#include "gameplay/tilemap.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/renderer.h"
#include "input.h"
#include "maths/shapes.h"
#include "platform.h"
#include <glm/gtc/matrix_transform.hpp>

uint64_t prev_ticks = 0;
float elapsed = 0.0f; // In seconds

int main()
{
    using namespace Uboat;

    Platform::init();
    Renderer renderer;

    Tilemap map("tilemap");
    Scene scene(&map);

    glm::mat4 matrix = glm::ortho(0.0f, 320.0f, 0.0f, 180.0f);

    prev_ticks = Platform::ticks();

    while (Platform::update())
    {
        // Calculate elapsed time
        uint64_t current_ticks = Platform::ticks();
        uint64_t tick_diff = current_ticks - prev_ticks;
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

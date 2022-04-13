#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "debug.h"
#include "gameplay/animator.h"
#include "gameplay/chaser.h"
#include "gameplay/collider.h"
#include "gameplay/explosion.h"
#include "gameplay/hurtable.h"
#include "gameplay/mover.h"
#include "gameplay/player.h"
#include "gameplay/playerhud.h"
#include "gameplay/tilemap.h"
#include "gameplay/torpedo.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/renderer.h"
#include "input.h"
#include "maths/shapes.h"
#include "platform.h"

int main()
{
    using namespace ITD;

    // Register all components
    Scene::register_component<Collider>();
    Scene::register_component<Mover>(Property::Updatable);
    Scene::register_component<Player>(Property::Updatable |
                                      Property::Renderable);
    Scene::register_component<Chaser>(Property::Updatable |
                                      Property::Renderable);
    Scene::register_component<Torpedo>(Property::Updatable |
                                       Property::Renderable);
    Scene::register_component<Hurtable>(Property::Updatable);
    Scene::register_component<PlayerHUD>(Property::HUD);
    Scene::register_component<Explosion>(Property::Updatable |
                                         Property::Renderable);
    Scene::register_component<Animator>(Property::Updatable |
                                        Property::Renderable);

    Platform::init();
    Renderer renderer;

    Tilemap map("Level_0");
    Scene scene(&map);

    glm::mat4 screen_matrix = glm::ortho(0.0f, Graphics::pixel_screen_size.x,
                                         0.0f, Graphics::pixel_screen_size.y);

    uint64_t prev_ticks = Platform::ticks();
    float elapsed = 0.0f;  // In seconds

    Color bg_color(0x29366f);

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
        Graphics::clear(bg_color);

        if (Input::keyboard()->pressed[SDL_SCANCODE_F12])
        {
            scene.toggle_debug_mode();
        }

        if (Input::keyboard()->pressed[SDL_SCANCODE_F11])
        {
            Platform::toggle_fullscreen();
        }

        renderer.begin();
        scene.render(&renderer);
        scene.render_hud(&renderer);
        renderer.end();

        renderer.render(screen_matrix);

        Graphics::present();
    }

    Platform::shutdown();
}

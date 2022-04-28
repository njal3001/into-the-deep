#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "debug.h"
#include "gameplay/animator.h"
#include "gameplay/camera.h"
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
#include "sound.h"
#include "gameplay/wall.h"

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
    Scene::register_component<Wall>(Property::Renderable);

    Platform::init();
    Renderer renderer;

    Tilemap map("Level_0");

    Rectf screen_bounds = Camera::make_fit(
        glm::vec2(map.pixel_width(), map.pixel_height()), 1920.0f / 1080.0f);

    Scene scene(&map, screen_bounds);

    glm::mat4 screen_matrix =
        glm::ortho(screen_bounds.bl.x, screen_bounds.tr.x, screen_bounds.bl.y,
                   screen_bounds.tr.y);

    uint64_t prev_ticks = Platform::ticks();
    float elapsed = 0.0f;  // In seconds

    Color bg_color(0x0);

    Platform::show_cursor(false);
    Platform::toggle_mute();

    // Avoid physics problems by capping the
    // elapsed time
    float max_elapsed = 1.0f / 60.0f;

    while (Platform::update())
    {
        // Calculate elapsed time
        uint64_t current_ticks = Platform::ticks();
        uint64_t tick_diff = current_ticks - prev_ticks;
        elapsed = (tick_diff) / (float)Platform::ticks_per_sec;
        elapsed = std::min(elapsed, max_elapsed);
        prev_ticks = current_ticks;

        if (Input::keyboard()->pressed[SDL_SCANCODE_F12])
        {
            scene.toggle_debug_mode();
        }

        if (Input::keyboard()->pressed[SDL_SCANCODE_F11])
        {
            Platform::toggle_fullscreen();
        }

        if (Input::keyboard()->pressed[SDL_SCANCODE_F10])
        {
            Platform::toggle_mute();
        }

        // Update
        scene.update(elapsed);

        // Render
        Graphics::clear(bg_color);

        renderer.begin();
        scene.render(&renderer);
        scene.render_hud(&renderer);
        renderer.end();

        renderer.render(screen_matrix);

        Graphics::present();
    }

    Platform::shutdown();
}

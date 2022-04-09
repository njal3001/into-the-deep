#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "gameplay/animator.h"
#include "gameplay/chaser.h"
#include "gameplay/collider.h"
#include "gameplay/explosion.h"
#include "gameplay/hurtable.h"
#include "gameplay/mover.h"
#include "gameplay/player.h"
#include "gameplay/playerhud.h"
#include "gameplay/rocket.h"
#include "gameplay/tilemap.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/renderer.h"
#include "input.h"
#include "maths/shapes.h"
#include "platform.h"

constexpr float screen_width = 540.0f;
constexpr float screen_height = 360.f;

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
    Scene::register_component<Rocket>(Property::Updatable |
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

    glm::mat4 matrix = glm::ortho(0.0f, screen_width, 0.0f, screen_height);
    glm::vec3 screen_center =
        glm::vec3(screen_width, screen_height, 0.0f) / 2.0f;
    const float map_pwidth = map.width() * 8.0f;
    const float map_pheight = map.height() * 8.0f;

    uint64_t prev_ticks = Platform::ticks();
    float elapsed = 0.0f;  // In seconds

    Color bg_color(0x29366f);

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
        Graphics::clear(bg_color);

        // Player camera
        glm::mat4 camera = glm::mat4(1.0f);
        auto pfirst = scene.first<Player>();
        if (pfirst != scene.end<Player>())
        {
            Player *player = (Player *)*pfirst;
            glm::vec3 offset =
                screen_center - glm::vec3(player->entity()->get_pos(), 1.0f);
            offset.x = std::clamp(offset.x, screen_width - map_pwidth, 0.0f);
            offset.y = std::clamp(offset.y, screen_height - map_pheight, 0.0f);
            camera = glm::translate(camera, offset);
        }

        renderer.begin();
        renderer.push_matrix(camera);
        scene.render(&renderer);
        renderer.pop_matrix();
        scene.render_hud(&renderer);
        renderer.end();

        renderer.render(matrix);

        Graphics::present();
    }

    Platform::shutdown();
}

#include "platform.h"
#include "input.h"
#include "graphics/graphics.h"
#include "graphics/renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/image.h"
#include "gameplay/tilemap.h"
#include "maths/shapes.h"

int main()
{
    using namespace Uboat;

    Platform::init();
    Renderer renderer;

    Image img("charger.png");
    Texture texture(img);

    Scene scene;
    Tilemap map("tilemap");

    map.load(&scene);

    Rectf rect(glm::vec2(50.0f, 50.0f), glm::vec2(20.0f, 40.0f));
    float rotation = 0.0f;

    glm::mat4 matrix = glm::ortho(0.0f, 320.0f, 0.0f, 180.0f);
    while (Platform::update())
    {
        // Game loop
        Quadf quad(rect, rotation);
        rotation += 0.01f;
        Graphics::clear(Color::blue);

        renderer.begin();
        /* renderer.tri(glm::vec2(0.0f, 0.0f), glm::vec2(160.0f, 180.0f), glm::vec2(320.0f, 0.0f), */
        /*         Color::white); */

        /* renderer.rect(glm::vec2(0.0f, 0.0f), glm::vec2(160.0f, 90.0f), Color::green); */
        /* renderer.circ(glm::vec2(160.0f, 90.0f), 10.0f, 128, Color::red); */
        /* renderer.tex(&texture, glm::vec2(0.0f, 0.0f), Color::white); */
        /* renderer.tex(&texture, glm::vec2(100.0f, 0.0f), Color::blue); */
        /* renderer.circ(glm::vec2(10.0f, 10.0f), 10.0f, 128, Color::red); */
        map.render(&renderer);
        renderer.quad(quad.a, quad.b, quad.c, quad.d, Color::black);
        renderer.end();

        renderer.render(matrix);

        Graphics::present();
    }

    Platform::shutdown();
}

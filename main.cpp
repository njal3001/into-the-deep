#include "platform.h"
#include "input.h"
#include "graphics/graphics.h"
#include "graphics/renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/image.h"

int main()
{
    using namespace Uboat;

    Platform::init();
    Renderer renderer;

    Image img("charger.png");
    Texture texture(img);

    glm::mat4 matrix = glm::ortho(0.0f, 320.0f, 0.0f, 180.0f);
    while (Platform::update())
    {
        // Game loop
        Graphics::clear(Color::blue);

        renderer.begin();
        renderer.tri(glm::vec2(0.0f, 0.0f), glm::vec2(160.0f, 180.0f), glm::vec2(320.0f, 0.0f),
                Color::white);

        renderer.rect(glm::vec2(0.0f, 0.0f), glm::vec2(160.0f, 90.0f), Color::green);
        renderer.circ(glm::vec2(160.0f, 90.0f), 10.0f, 128, Color::red);
        renderer.tex(texture, glm::vec2(0.0f, 0.0f), Color::white);
        renderer.end();

        renderer.render(matrix);

        Graphics::present();
    }

    Platform::shutdown();
}

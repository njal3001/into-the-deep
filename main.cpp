#include "platform.h"
#include "graphics/graphics.h"

int main()
{
    using namespace Uboat;

    Platform::init();

    while (Platform::update())
    {
        // Game loop
        Graphics::clear(Color::blue);
        Graphics::present();
    }

    Platform::shutdown();
}

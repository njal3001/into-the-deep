#include "graphics.h"
#include "../platform.h"
#include <GL/glew.h>
#include <SDL2/SDL_video.h>

namespace Uboat
{
    namespace
    {
        void *g_context = nullptr;
    }

    bool Graphics::init()
    {
        g_context = SDL_GL_CreateContext(Platform::get_window());
        if (!g_context)
        {
            return false;
        }

        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
            return false;
        }

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return true;
    }

    void Graphics::clear(const Color color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Graphics::present()
    {
        SDL_GL_SwapWindow(Platform::get_window());
    }

    void Graphics::shutdown()
    {
        SDL_GL_DeleteContext(g_context);
        g_context = nullptr;
    }
}

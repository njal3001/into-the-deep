#include "graphics.h"
#include "../platform.h"
#include <GL/glew.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#define APIENTRY
#endif

namespace ITD
{
    void APIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* message, const void* user_param)
    {
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && type == GL_DEBUG_TYPE_OTHER)
        {
            return;
        }

        printf("%s", message);
    }

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

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_message_callback, nullptr);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return true;
    }

    void Graphics::clear(const Color color)
    {
        glClearColor(color.r / 255.0f, 
                color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
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

#include <string>

#include "window.h"
#include "api/logging/logging.h"

using namespace std::literals;

namespace projectfarm::shared::api::windowing::windows
{
    bool Window::LoadFromConfig(const std::filesystem::path& configPath) noexcept
    {
        auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                     SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        this->_window = SDL_CreateWindow("title",
                                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         500, 500,
                                         flags);
        if (!this->_window)
        {
            api::logging::Log("Failed to create window: "s + SDL_GetError());
            return false;
        }

        this->_context = SDL_GL_CreateContext(this->_window);
        if (!this->_context)
        {
            api::logging::Log("Failed to create GL context: "s + SDL_GetError());
            return false;
        }

        return true;
    }

    void Window::Shutdown() noexcept
    {
        if (this->_context)
        {
            SDL_GL_DeleteContext(this->_context);
            this->_context = nullptr;
        }

        if (this->_window)
        {
            SDL_DestroyWindow(this->_window);
            this->_window = nullptr;
        }
    }
}

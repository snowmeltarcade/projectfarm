#ifndef PROJECTFARM_WINDOW_H
#define PROJECTFARM_WINDOW_H

#include <SDL.h>

#include "iwindow.h"

namespace projectfarm::shared::api::windowing::windows
{
    class Window final : public IWindow
    {
    public:
        Window() = default;
        ~Window() override
        {
            this->Shutdown();
        }

        Window(const Window&) = delete;
        Window(Window&&) = default;

        [[nodiscard]]
        bool LoadFromConfig(const std::filesystem::path& configPath) noexcept override;

    private:
        SDL_Window* _window {nullptr};
        SDL_GLContext _context {nullptr};

        void Shutdown() noexcept;
    };
}

#endif

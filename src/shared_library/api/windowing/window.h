#ifndef PROJECTFARM_WINDOW_H
#define PROJECTFARM_WINDOW_H

#include "iwindow.h"

namespace projectfarm::shared::api::windowing
{
    class Window final : public IWindow
    {
    public:
        Window() = default;
        ~Window() override
        {
            this->ShutdownSDL();
        }

        Window(const Window&) = delete;
        Window(Window&&) = default;

        [[nodiscard]]
        bool LoadFromConfig(const std::filesystem::path& configPath) noexcept override;

    private:
        [[nodiscard]]
        bool SetupSDL() noexcept;

        void ShutdownSDL() noexcept;
    };
}

#endif

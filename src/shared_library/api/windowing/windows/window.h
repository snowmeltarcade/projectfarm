#ifndef PROJECTFARM_WINDOW_H
#define PROJECTFARM_WINDOW_H

#include "iwindow.h"

namespace projectfarm::shared::api::windowing::windows
{
    class Window final : public IWindow
    {
    public:
        Window() = default;
        ~Window() override
        {
        }

        Window(const Window&) = delete;
        Window(Window&&) = default;

        [[nodiscard]]
        bool LoadFromConfig(const std::filesystem::path& configPath) noexcept override;
    };
}

#endif

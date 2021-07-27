#ifndef PROJECTFARM_IWINDOW_H
#define PROJECTFARM_IWINDOW_H

#include <string>
#include <filesystem>

namespace projectfarm::shared::api::windowing::windows
{
    class IWindow
    {
    public:
        IWindow() = default;
        virtual ~IWindow() = default;
        IWindow(const IWindow&) = delete;
        IWindow(IWindow&&) = default;

        [[nodiscard]]
        virtual bool LoadFromConfig(const std::filesystem::path& configPath) noexcept = 0;

        virtual void PumpMessages() = 0;

        [[nodiscard]]
        virtual bool SetTitle(std::string_view title) = 0;
    };
}

#endif

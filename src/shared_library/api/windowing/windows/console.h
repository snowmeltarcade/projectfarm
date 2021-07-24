#ifndef PROJECTFARM_CONSOLE_H
#define PROJECTFARM_CONSOLE_H

#include "iwindow.h"

namespace projectfarm::shared::api::windowing::windows
{
    class Console final : public IWindow
    {
    public:
        Console() = default;
        ~Console() override = default;
        Console(const Console&) = delete;
        Console(Console&&) = default;

        [[nodiscard]]
        bool LoadFromConfig(const std::filesystem::path& configPath) noexcept override
        {
            return true;
        }

        void PumpMessages() override
        {
        }
    };
}

#endif

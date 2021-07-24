#ifndef PROJECTFARM_SERVICE_H
#define PROJECTFARM_SERVICE_H

#include "iwindow.h"

namespace projectfarm::shared::api::windowing::windows
{
    // used for running as a service/daemon
    class Service final : public IWindow
    {
    public:
        Service() = default;
        ~Service() override = default;
        Service(const Service&) = delete;
        Service(Service&&) = default;

        [[nodiscard]]
        bool LoadFromConfig(const std::filesystem::path& configPath) noexcept override
        {
            return true;
        }

        void PumpMessages() override
        {
        }

        [[nodiscard]]
        bool SetTitle(std::string_view title) override
        {
            return true;
        }
    };
}

#endif

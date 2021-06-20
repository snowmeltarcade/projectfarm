#ifndef PROJECTFARM_CLIENT_CONFIG_H
#define PROJECTFARM_CLIENT_CONFIG_H

#include <cstdint>
#include <string>

#include "data/consume_data_provider.h"
#include "logging/consume_logger.h"

namespace projectfarm::engine
{
    class ClientConfig final : public shared::ConsumeDataProvider,
                               public shared::ConsumeLogger
    {
    public:
        ClientConfig() = default;
        ~ClientConfig() override = default;

        [[nodiscard]]
        bool LoadConfig() noexcept;

        [[nodiscard]]
        uint16_t GetTcpPort() const noexcept
        {
            return this->_tcpPort;
        }

        [[nodiscard]]
        uint16_t GetClientUdpPort() const noexcept
        {
            return this->_clientUdpPort;
        }

        [[nodiscard]]
        uint16_t GetServerUdpPort() const noexcept
        {
            return this->_serverUdpPort;
        }

        [[nodiscard]]
        std::string GetHostName() const noexcept
        {
            return this->_hostName;
        }

        [[nodiscard]]
        uint32_t GetScreenWidthInMeters() const noexcept
        {
            return this->_screenWidthInMeters;
        }

    private:
        uint16_t _tcpPort {0};
        uint16_t _clientUdpPort {0};
        uint16_t _serverUdpPort {0};
        std::string _hostName;

        uint32_t _screenWidthInMeters {10};
    };
}

#endif

#ifndef PROJECTFARM_SERVER_CONFIG_H
#define PROJECTFARM_SERVER_CONFIG_H

#include <cstdint>
#include <string>

#include "data/consume_data_provider.h"
#include "logging/consume_logger.h"

namespace projectfarm::server
{
    class ServerConfig final : public shared::ConsumeDataProvider,
                               public shared::ConsumeLogger
    {
    public:
        ServerConfig() = default;
        ~ServerConfig() override = default;

        [[nodiscard]] bool LoadConfig();

        [[nodiscard]] uint16_t GetTcpPort() const noexcept
        {
            return this->_tcpPort;
        }

        [[nodiscard]] uint16_t GetServerUdpPort() const noexcept
        {
            return this->_serverUdpPort;
        }

        [[nodiscard]] const std::string& GetStartingWorld() const noexcept
        {
            return this->_startingWorld;
        }

    private:
        uint16_t _tcpPort {0};
        uint16_t _serverUdpPort {0};

        std::string _startingWorld;
    };
}

#endif

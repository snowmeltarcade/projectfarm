#include <fstream>
#include <nlohmann/json.hpp>

#include "client_config.h"
#include "api/logging/logging.h"

namespace projectfarm::engine
{
    bool ClientConfig::LoadConfig() noexcept
    {
        shared::api::logging::Log("Loading client config...");

        auto filePath = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::Client, "config.json");

        shared::api::logging::Log("Loading client config from " + filePath.u8string());

        std::ifstream file(filePath);

        if (!file.is_open())
        {
            return false;
        }

        nlohmann::json jsonFile;

        file >> jsonFile;

        this->_tcpPort = jsonFile["tcpPort"].get<uint16_t>();
        this->_clientUdpPort = jsonFile["clientUdpPort"].get<uint16_t>();
        this->_serverUdpPort = jsonFile["serverUdpPort"].get<uint16_t>();
        this->_hostName = jsonFile["hostName"].get<std::string>();

        this->_screenWidthInMeters = jsonFile["screenWidthInMeters"].get<uint32_t>();

        shared::api::logging::Log("Loaded client config.");

        return true;
    }
}
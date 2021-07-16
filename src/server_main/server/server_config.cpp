#include <fstream>
#include <nlohmann/json.hpp>

#include "server_config.h"
#include "data/data_provider_locations.h"
#include "api/logging/logging.h"

namespace projectfarm::server
{
    bool ServerConfig::LoadConfig()
    {
        shared::api::logging::Log("Loading server config...");

        auto filePath = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::Server, "config.json");

        shared::api::logging::Log("Loading server config from " + filePath.u8string());

        std::ifstream file(filePath);

        if (!file.is_open())
        {
            return false;
        }

        nlohmann::json jsonFile;

        file >> jsonFile;

        this->_tcpPort = jsonFile["tcpPort"].get<uint16_t>();
        this->_serverUdpPort = jsonFile["serverUdpPort"].get<uint16_t>();
        this->_startingWorld = jsonFile["startingWorld"].get<std::string>();

        shared::api::logging::Log("Loaded server config.");

        return true;
    }
}
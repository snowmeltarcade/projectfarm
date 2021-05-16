#include <fstream>
#include <nlohmann/json.hpp>

#include "server_config.h"
#include "data/data_provider_locations.h"

namespace projectfarm::server
{
    bool ServerConfig::LoadConfig()
    {
        this->LogMessage("Loading server config...");

        auto filePath = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::Server, "config.json");

        this->LogMessage("Loading server config from " + filePath.u8string());

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

        this->LogMessage("Loaded server config.");

        return true;
    }
}
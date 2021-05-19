#include "client_config.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace projectfarm::engine
{
    bool ClientConfig::LoadConfig() noexcept
    {
        this->LogMessage("Loading client config...");

        auto filePath = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::Client, "config.json");

        this->LogMessage("Loading client config from " + filePath.u8string());

        std::ifstream file(filePath);

        if (!file.is_open())
            return false;

        nlohmann::json jsonFile;

        file >> jsonFile;

        this->_tcpPort = jsonFile["tcpPort"].get<uint16_t>();
        this->_clientUdpPort = jsonFile["clientUdpPort"].get<uint16_t>();
        this->_serverUdpPort = jsonFile["serverUdpPort"].get<uint16_t>();
        this->_hostName = jsonFile["hostName"].get<std::string>();

        this->_screenWidth = jsonFile["screenWidth"].get<uint32_t>();
        this->_screenHeight = jsonFile["screenHeight"].get<uint32_t>();
        this->_fullScreen = jsonFile["fullScreen"].get<bool>();

        this->_screenWidthInMeters = jsonFile["screenWidthInMeters"].get<uint32_t>();

        this->LogMessage("Loaded client config.");

        return true;
    }
}